/**
 * Copyright (c) 2019-2021 Chris Hiszpanski. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 */

#include <assert.h>                     // assert
#include <errno.h>                      // errno
#include <poll.h>                       // poll
#include <pthread.h>                    // pthread_create
#include <stdbool.h>                    // true
#include <stdio.h>                      // fprintf
#include <stdlib.h>                     // calloc, free
#include <string.h>                     // strerror
#include <unistd.h>                     // close

#include <arpa/inet.h>                  // inet_ntoa
#include <netinet/in.h>                 // IPPROTO_UDP
#include <sys/socket.h>                 // socket
#include <sys/types.h>

#include "err.h"
#include "log.h"
#include "mdns.h"                       // mdns_subscribe, mdns_unsubscribe
#include "prng.h"                       // prng_init
#include "sdp.h"
#include "urtc.h"
#include "uuid.h"                       // uuid_create_str

#define RX_BUF_CAP               2048   // receive buffer capacity

#define POLL_TIMEOUT_MS            50   // poll() timeout (in milliseconds)

const static char *default_stun_servers[] = {
    "stun.liburtc.org",
    NULL
};

enum signaling_state {
    SIGNALING_STATE_STABLE = 0,
    SIGNALING_STATE_HAVE_LOCAL_OFFER,
    SIGNALING_STATE_HAVE_REMOTE_OFFER,
    SIGNALING_STATE_HAVE_LOCAL_PRANSWER,
    SIGNALING_STATE_HAVE_REMOTE_PRANSWER,
};

enum rtc_state {
    STATE_NEW = 0,

    STATE_ICE_NEW,
    STATE_ICE_GATHERING,
    STATE_ICE_COMPLETE,

    STATE_CONNECTING,

    STATE_CONNECTED,

    STATE_DISCONNECTED,

    STATE_FAILED,

    STATE_CLOSED,

    NUM_STATES // must be last
};

enum rtc_event {
    EVENT_SOCKET = 0,
    EVENT_TIMER,
    EVENT_MDNS,
    NUM_EVENTS, // must be last
};

struct peerconn {
    // socket file descriptor
    int sockfd;

    // execution thread
    pthread_t thread;

    // poll file descriptors
    struct pollfd fds[NUM_EVENTS];

    // callbacks
    urtc_on_ice_candidate *on_ice_candidate;
    urtc_force_idr *force_idr;

    // stun servers
    const char **stun;

    // state machine
    enum signaling_state ss;

    // local and remote descriptions
    struct sdp ldesc, rdesc;

    // mDNS related state
    struct {
        char hostname[UUID_STR_LEN];    // .local hostname
        int  sockfd;                    // port 5353 listener
    } mdns;
};


typedef int (*event_handler)(struct peerconn *pc);

const enum rtc_state state_table[NUM_STATES][NUM_EVENTS] = {
    { STATE_NEW, STATE_NEW },
    { STATE_NEW, STATE_NEW },
    { STATE_NEW, STATE_NEW },
    { STATE_NEW, STATE_NEW },
    { STATE_NEW, STATE_NEW },
    { STATE_NEW, STATE_NEW },
    { STATE_NEW, STATE_NEW },
    { STATE_NEW, STATE_NEW },
    { STATE_NEW, STATE_NEW }
};

/**
 * Handle incoming STUN packet
 *
 * \param pc Peer connection.
 *
 * \return 0 on success, negative on error.
 */
static int stun_handler(
    struct peerconn *pc,
    const uint8_t *pkt,
    size_t n
) {
    return 0;
}

/**
 * Handle incoming DTLS packet
 *
 * \param pc Peer connection.
 *
 * \return 0 on success, negative on error.
 */
static int dtls_handler(
    struct peerconn *pc,
    const uint8_t *pkt,
    size_t n
) {
    return 0;
}

/**
 * Handle incoming SRTP (or SRTCP) packet
 *
 * \param pc Peer connection.
 *
 * \return 0 on success, negative on error.
 */
static int rtp_handler(
    struct peerconn *pc,
    const uint8_t *pkt,
    size_t n
) {
    return 0;
}

/**
 * Handle incoming packet
 *
 * Packet may be a DTLS, SRTP, SRTCP, or STUN packet. Other packet types
 * are discarded.
 *
 * \param pc Peer connection.
 *
 * \return 0 on success, negative on error.
 */
static int socket_event_handler(struct peerconn *pc) {
    uint8_t buffer[RX_BUF_CAP];
    struct sockaddr_in ra;
    socklen_t ralen;
    ssize_t n;

    // read packet
    ralen = sizeof(ra);
    n = recvfrom(
        pc->sockfd,
        buffer,
        sizeof(buffer),
        0,
        (struct sockaddr *)(&ra),
        &ralen
    );

    // rtp
    if ((127 < buffer[0]) && (buffer[0] < 192)) {
        log(INFO, "[rtp] %s", inet_ntoa(ra.sin_addr));
        rtp_handler(pc, buffer, n);
    } else
    // dtls
    if ((19 < buffer[0]) && (buffer[0] < 64)) {
        log(INFO, "[dtls] %s", inet_ntoa(ra.sin_addr));
        dtls_handler(pc, buffer, n);
    } else
    // stun
    if (buffer[0] < 2) {
        log(INFO, "[stun] %s", inet_ntoa(ra.sin_addr));
        stun_handler(pc, buffer, n);
    }

    return 0;
}

/**
 * Handle timer event
 *
 * May need to:
 * - resend STUN packet
 * - resend DTLS packet
 * - expire ICE candidate?
 *
 * \param pc Peer connection.
 *
 * \return 0 on success, negative on error.
 */
static int timer_event_handler(struct peerconn *pc) {
    return 0;
}

/**
 * Handle incoming mDNS query
 *
 * \param pc Peer connection.
 *
 * \return 0 on success, negative on error.
 */
static int mdns_handler(struct peerconn *pc) {
    uint8_t buffer[RX_BUF_CAP];
    ssize_t n;

    // read packet
    if (n = recvfrom(
        pc->mdns.sockfd,
        buffer,
        sizeof(buffer),
        0,
        NULL,
        NULL
    ), -1 == n) {
        log(ERROR, "%s", strerror(errno));
        goto _fail_recvfrom;
    }

    // if valid query for peer connection's ephemeral hostname...
    int type = mdns_validate_query(buffer, n, pc->mdns.hostname);
    if (type > 0) {
        if (type | TYPE_A) {
            log(DEBUG, "[mdns] received A query");
            // ...respond to query with interface addresses
            mdns_send_response(pc->mdns.sockfd, pc->mdns.hostname);
        }
        if (type | TYPE_AAAA) {
            log(DEBUG, "[mdns] received AAAA query");
        }
    }

    return 0;

_fail_recvfrom:
    return -URTC_ERR;
}

const event_handler action_table[NUM_STATES][NUM_EVENTS] = {
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    { NULL, NULL, NULL },
    { NULL, NULL, NULL }
};

/**
 * Runloop finite state machine (mealy) for handling timer and receive events
 */

// send stun request
// wait for stun reply
//      timeout -> resend request

// called                           dir         caller
// ------                           ---         ------
// setRemoteDescription(offer)      <--         offer = createOffer()
//                                              setLocalDescription(offer)
// answer = createAnswer()          -->         setRemoteDescription(answer)
// setLocalDescription(answer)
//
// cand = onIceCandidate            -->         addIceCandidate(cand)
// addIceCandidate(cand)            <--         cand = onIceCandidate
//                                  ...
//
//                       (connection established)
//
//                           (dtls handshake)
//
// srtp -->
// <-- rtcp, nack

/**
 * Execution thread per peer connection
 *
 * \param arg Peer connection
 *
 * \return Unused.
 */
static void * runloop(void *arg) {
    struct peerconn *pc;
    enum rtc_event event;
    int n;

    pc = (struct peerconn *)arg;
    assert(pc);

_loop:
    // block indefinitely (-1 timeout) until i/o event
    n = poll(pc->fds, NUM_EVENTS, -1);
    assert(n > 0);

    // which event(s) occurred?
    if (pc->fds[EVENT_SOCKET].revents & POLLIN) {
        event = EVENT_SOCKET;
    } else
    if (pc->fds[EVENT_MDNS].revents & POLLIN) {
        mdns_handler(pc);
    } else
    if (pc->fds[EVENT_TIMER].revents & POLLIN) {
        event = EVENT_TIMER;
    }

    goto _loop;


    return NULL;
};

urtc_peerconn_t * urtc_peerconn_create(const char *stun[]) {
    // seed pseudorandom number generator
    prng_init();

    // allocate peer connection
    struct peerconn *pc = (struct peerconn *)calloc(1, sizeof(struct peerconn));
    if (!pc) return pc;

    // copy pointer to stun servers
    if (!stun) {
        pc->stun = default_stun_servers;
    } else {
        pc->stun = stun;
    }

    // open udp socket (for all dtls/srtp/srtcp/stun/turn communication)
    pc->sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (-1 == pc->sockfd) goto _fail_socket;
    pc->fds[EVENT_SOCKET] = (struct pollfd){ pc->sockfd, POLLIN };

    // generate a unique local mDNS hostname
    uuid_create_str(pc->mdns.hostname);
    log(INFO, "mDNS hostname is %s.local", pc->mdns.hostname);

    // open multicast udp socket for replying to mDNS queries
    pc->mdns.sockfd = mdns_subscribe();
    if (-1 == pc->mdns.sockfd) goto _fail_mdns_subscribe;
    pc->fds[EVENT_MDNS] = (struct pollfd){
        pc->mdns.sockfd,
        POLLIN
    };

    // start new thread per peer connection
    if (0 != pthread_create(
        &(pc->thread), 
        NULL,
        runloop,
        pc
    )) goto _fail_pthread_create;

    return pc;

_fail_pthread_create:
    mdns_unsubscribe(pc->mdns.sockfd);
_fail_mdns_subscribe:
    close(pc->sockfd);
_fail_socket:
    free(pc);

    return NULL;
}

int urtc_set_on_ice_candidate(struct peerconn *pc, urtc_on_ice_candidate *cb) {
    if (!pc) return -1;
    if (!cb) return -1;
    pc->on_ice_candidate = cb;

    return -URTC_ERR_NOT_IMPLEMENTED;
}

int urtc_add_ice_candidate(struct peerconn *pc, const char *cand) {
    return -URTC_ERR_NOT_IMPLEMENTED;
}

int urtc_create_answer(struct peerconn *pc, char **answer) {
    return -URTC_ERR_NOT_IMPLEMENTED;
}

int urtc_create_offer(struct peerconn *pc, char **offer) {
    return -URTC_ERR_NOT_IMPLEMENTED;
}

int urtc_set_remote_description(struct peerconn *pc, const char *desc) {
    return -URTC_ERR_NOT_IMPLEMENTED;
}

int urtc_set_local_description(struct peerconn *pc, const char *desc) {
    return -URTC_ERR_NOT_IMPLEMENTED;
}

void urtc_peerconn_destroy(struct peerconn *pc) {
    if (pc) {
        pthread_cancel(pc->thread);
        pthread_join(pc->thread, NULL);
        mdns_unsubscribe(pc->mdns.sockfd);
        shutdown(pc->sockfd, SHUT_RDWR);
        close(pc->sockfd);
        free(pc);
    }
}

/* vim: set expandtab ts=8 sw=4 tw=0 : */
