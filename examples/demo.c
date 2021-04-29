/**
 * Simple callee example (web browser is caller). Sends video to caller upon
 * incoming calls.
 */

#include <errno.h>                      // errno
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>                      // printf
#include <string.h>
#include <unistd.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "urtc.h"

// STUN servers
const char *stun[] = {
	"stun.liburtc.org",
	NULL
};

static int http_connect(const char *hostname) {
    int err, fd;
	struct addrinfo *res, *res0;

	struct addrinfo hints = {
		.ai_family   = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_protocol = IPPROTO_TCP
	};

    // resolve hostname to IPv4/IPv6 address
	if (err = getaddrinfo(hostname, "http", &hints, &res0), err) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(errno));
        return -1;
	}

	for (res = res0; res; res = res->ai_next) {
		// attempt to open socket
		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (-1 == fd) {
			continue; // failed. onto the next result.
		}

		// attempt to connect socket
		if (-1 == connect(fd, res->ai_addr, res->ai_addrlen)) {
		    close(fd);
		    fd = -1;
		    continue; // failed. onto the next result.
		}

        break; // success. return.
	}

	// free resolved addresses
	freeaddrinfo(res0);

    return fd;
}

static int http_post(
    const char *hostname,
    const char *path,
    const char *body
) {
    const char *tmpl = 
        "POST %s HTTP/1.0\r\n"
        "Host: %s\r\n"
        "Content-Length: %d\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n";

    int fd, rv = -1;
    char req[128];

    if (fd = http_connect(hostname), -1 == fd) {
        return -1;
    }

    // construct request
    if (-1 == snprintf(req, sizeof(req), tmpl, path, hostname, strlen(body))) {
        fprintf(stderr, "bad request template\n");
        goto _unwind_socket;
    }

    // send request
    if (rv = send(fd, req, strlen(req), 0), -1 == rv) {
        perror("send");
        goto _unwind_socket;
    }

    // send request body
    if (rv = send(fd, body, strlen(body), 0), -1 == rv) {
        perror("send");
        goto _unwind_socket;
    }

_unwind_socket:
    shutdown(fd, SHUT_RDWR);
    close(fd);

_done:
    return rv;
}

/**
 * HTTP GET request
 *
 * Resolves simple signaling server hostname to IPv4/IPv6 address and connects
 * to its HTTP port.
 *
 * \param[out] response Buffer into which response is written.
 * \param[in] size Max size of response.
 * \param hostname Server hostname (e.g. "demo.liburtc.org")
 * \param path HTTP path (e.g. "/offer")
 *
 * \return Socket file descriptor, or -1 on error. Caller must close file
 *         descriptor.
 */
static int get(
    char *response,
    size_t size,
    const char *hostname,
    const char *path
) {
    const char *tmpl = 
        "GET %s HTTP/1.0\r\n"
        "Host: %s\r\n"
        "\r\n";

    int err, fd, rv = -1;
    char req[128];

    if (fd = http_connect(hostname), -1 == fd) {
        return -1;
    }

    // construct request
    if (-1 == snprintf(req, sizeof(req), tmpl, path, hostname)) {
        fprintf(stderr, "bad request template\n");
        goto _unwind_socket;
    }

    // send request
    if (rv = send(fd, req, strlen(req), 0), -1 == rv) {
        perror("send");
        goto _unwind_socket;
    }

    // get response
    if (rv = recv(fd, response, size, 0), -1 == rv) {
        perror("recv");
        goto _unwind_socket;
    }

_unwind_socket:
    shutdown(fd, SHUT_RDWR);
    close(fd);

_done:
    return rv;
}


/**
 * (callback) Handles incoming ICE candidates.
 *
 * \param cand[in] ICE candidate.
 * \param arg[in] User argument.
 */
void handle_ice_candidate(const char *cand, void *arg) {
    urtc_peerconn_t *pc = (urtc_peerconn_t *)arg;

    urtc_add_ice_candidate(pc, cand);
}

static int longpoll(char *response, size_t size, const char *path) {
    int n;

    while (true) {
        n = get(response, size, "demo.liburtc.org", path);
        if (-1 == n || 0 == n) {
            perror("get");
            continue;
        }

        // blindly strip http headers
        char *body = strstr(response, "\r\n\r\n");
        strcpy(response, body + 4);

        // ignore empty responses (i.e. long-poll timeout)
        if (0 == strlen(response)) {
            continue;
        }

        break;
    }

    return n;
}

char offer[4096];
char answer[4096];

int main(int argc, char **argv) {
    int n;

    // create a new peer connection (no actual network communication yet)
    urtc_peerconn_t *pc = urtc_peerconn_create(stun);

    // TODO add tracks here

    // get offer
    n = longpoll(offer, sizeof(offer), "/offer");
    fprintf(stderr, "%s\n", offer);

    if (urtc_set_remote_description(pc, offer)) {
        fprintf(stderr, "error: set remote description\n");
    }
    if (urtc_create_answer(pc, answer, sizeof(answer))) {
        fprintf(stderr, "error: create answer\n");
    }

    http_post("demo.liburtc.org", "/answer", answer);
    fprintf(stderr, "answer:\n%s\n", answer);
    //urtc_set_local_description(pc, answer);


    // block until signal
    {
	    int signal;
	    sigset_t ss;
	    sigemptyset(&ss);
	    sigaddset(&ss, SIGINT);
	    sigaddset(&ss, SIGTERM);
	    sigaddset(&ss, SIGQUIT);
	    sigwait(&ss, &signal);
    }

    // clean up
	urtc_peerconn_destroy(pc);

	return 0;
}

/* vim: set expandtab ts=4 sw=4 tw=0 : */
