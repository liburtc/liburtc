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

#include <errno.h>
#include <net/if.h>                     // IFF_LOOPBACK (include pre ifaddrs.h)
#include <ifaddrs.h>                    // getifaddrs
#include <limits.h>                     // HOST_NAME_MAX
#include <stdbool.h>
#include <string.h>                     // memcpy
#include <unistd.h>                     // sleep

#include <arpa/inet.h>
#include <netinet/in.h>
#ifdef WITH_IPV6
  #include <netinet6/in6.h>
#endif
#include <sys/socket.h>                 // sendto, setsocketopt, socket

#include "err.h"
#include "log.h"
#include "mdns.h"
#include "uuid.h"

#define MDNS_ADDR         "224.0.0.251"
#define MDNS_PORT                 5353

#define DEFAULT_TTL                120  // time-to-live recommended by RFC 6762

#define CLASS_INTERNET               1
#define CACHE_FLUSH           (1 << 15)

#define FLAG_RESPONSE         (1 << 15)
#define FLAG_OPCODE         (0xF << 11)

#ifndef HOST_NAME_MAX
  #define HOST_NAME_MAX            255
#endif

struct __attribute__((__packed__)) header {
    uint16_t id;
    uint16_t flags;
    uint16_t n_question;                // number of question records
    uint16_t n_answer;                  // number of answer records
    uint16_t n_authority;               // number of authority records
    uint16_t n_additional;              // number of additional records
};

struct __attribute__((__packed__)) qname {
    uint8_t size;
    uint8_t name[];
};

struct __attribute__((__packed__)) answer {
    // variable length name precedes answer
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t size;
    uint8_t  data[];
};

/**
 * Subscribe to mDNS queries
 *
 * Opens new multicast socket on the mDNS port. This socket receives and can
 * be used to send mDNS datagrams. Close with mdns_unsubscribe().
 *
 * \return Socket file descriptor on success, negative on error.
 */
int mdns_subscribe(void) {
    // create socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (-1 == sockfd) {
        urtc_log(URTC_ERROR, "%s", strerror(errno));
        goto _fail_create_socket;
    }

    // enable address/port reuse
    {
        const unsigned int opt = 1;
        if (-1 == setsockopt(
            sockfd,
            SOL_SOCKET,
#if defined(__APPLE__) || defined(__FreeBSD__)
            SO_REUSEPORT,
#else
            SO_REUSEADDR,
#endif
            &opt,
            sizeof(opt)
        )) {
            urtc_log(URTC_ERROR, "%s", strerror(errno));
            goto _fail_enable_addr_reuse;
        }
    }

    // bind socket to mDNS query port
    {
        const struct sockaddr_in mgroup = {
            .sin_family = AF_INET,
            .sin_addr.s_addr = htonl(INADDR_ANY),
            .sin_port = htons(MDNS_PORT)
        };
        if (-1 == bind(sockfd, (struct sockaddr *)(&mgroup), sizeof(mgroup))) {
            urtc_log(URTC_ERROR, "%s", strerror(errno));
            goto _fail_bind;
        }
    }

    // disable loopback
    {
        const int opt = 0;
        if (-1 == setsockopt(
            sockfd,
            IPPROTO_IP,
            IP_MULTICAST_LOOP,
            &opt,
            sizeof(opt)
        )) {
            urtc_log(URTC_ERROR, "setsockopt: %s", strerror(errno));
            goto _fail_disable_loopback;
        }
    }

    // join multicast group
    {
        const struct ip_mreqn imr = {
            .imr_multiaddr.s_addr = inet_addr(MDNS_ADDR),
            .imr_address.s_addr = htonl(INADDR_ANY)
        };
        if (-1 == setsockopt(
            sockfd,
            IPPROTO_IP,
            IP_ADD_MEMBERSHIP,
            &imr,
            sizeof(imr)
        )) {
            urtc_log(URTC_ERROR, "setsockopt: %s", strerror(errno));
            goto _fail_join_multicast_group;
        }
    }

    return sockfd;

_fail_join_multicast_group:
_fail_disable_loopback:
_fail_bind:
_fail_enable_addr_reuse:
    close(sockfd);
_fail_create_socket:

    return -URTC_ERR;
}

/**
 * Unsubscribe from mDNS queries
 *
 * Close multicast socket previously opened with mdns_subscribe().
 *
 * \param sockfd Socket file descriptor returned by mdns_subscribe().
 *
 * \return 0 on success, negative on error.
 */
int mdns_unsubscribe(int sockfd) {
    if (-1 == close(sockfd)) {
        urtc_log(URTC_ERROR, "close: %s", strerror(errno));
        return -URTC_ERR;
    }

    return 0;
}

/**
 * Send a multicast DNS query
 *
 * \param name Hostname to query. Must end with ".local".
 *
 * \return 0 on success, negative on error.
 */
int mdns_query(const char *name) {
    // create socket
    int msockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (-1 == msockfd) {
        urtc_log(URTC_FATAL, "socket: %s", strerror(errno));
    }

    uint8_t q[512];
    size_t qlen = 0;

    // header
    struct header hdr = {
        .n_question = htons(1)
    };
    memcpy(&q[qlen], &hdr, sizeof(hdr));
    qlen += sizeof(hdr);

    // qname
    q[qlen] = strlen(name);
    memcpy(&q[1 + qlen], name, strlen(name));
    qlen += 1 + q[qlen];

    q[qlen] = strlen("local");
    memcpy(&q[1 + qlen], "local", strlen("local"));
    qlen += 1 + q[qlen];

    q[qlen++] = 0; // root record

    // qtype: 255 = any
    q[qlen++] = 0;
    q[qlen++] = 255;

    // qclass: 1 = ipv4
    q[qlen++] = 0x80; // unicast response please
    q[qlen++] = 1;

    // send query
    struct sockaddr_in mgroup = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(MDNS_ADDR),
        .sin_port = htons(MDNS_PORT)
    };
    if (-1 == sendto(msockfd, q, qlen, 0, (struct sockaddr *)(&mgroup), sizeof(mgroup))) {
        urtc_log(URTC_FATAL, "sendto: %s", strerror(errno));
    }

    return 0;
}

/**
 * Simple mDNS response parser (single record only)
 *
 * \param response mDNS response packet
 * \param len Length of response, in bytes
 *
 * \return 0 on success, negative on error.
 */
int mdns_parse_response(const void *response, size_t len)
{
    // sanity checks
    if (!response) return -URTC_ERR_BAD_ARGUMENT;
    if (len < sizeof(struct header)) return -URTC_ERR_MALFORMED;

    struct header *hdr = (struct header *)(response);
    response += sizeof(struct header);
    len -= sizeof(struct header);

    // transaction id must be zero
    if (0 != hdr->id) return -URTC_ERR_MALFORMED;

    // flags must indicate a response
    if (!(ntohs(hdr->flags) & FLAG_RESPONSE)) return -URTC_ERR_MALFORMED;

    // sent single question, expecting reponse for single question
    if (1 != ntohs(hdr->n_question)) return -URTC_ERR_MALFORMED;

    // parse query names
    struct qname *qname;
    do {
        if (len < 1) return -URTC_ERR_MALFORMED;
        qname = (struct qname *)(response);
        if (len < 1 + qname->size) return -URTC_ERR_MALFORMED;
        response += 1 + qname->size;
        len -= 1 + qname->size;
    } while (qname->size);

    // parse query type (ignore)
    if (len < sizeof(uint16_t)) return -URTC_ERR_MALFORMED;
    response += sizeof(uint16_t);
    len -= sizeof(uint16_t);

    // parse query class (ignore)
    if (len < sizeof(uint16_t)) return -URTC_ERR_MALFORMED;
    response += sizeof(uint16_t);
    len -= sizeof(uint16_t);

    // answers
    for (int i = 0; i < ntohs(hdr->n_answer); i++) {
        uint16_t size;
        uint8_t namelen;

        // parse name (assume matches)
        if (len < 2) return -URTC_ERR_MALFORMED;
        namelen = *(uint8_t *)response;
        switch (namelen) {
            // name compression
            case 0xc0:
                response += 2;
                len -= 2;
                break;
            // no name compression
            default:
                response += 1 + namelen;
                len -= 1 + namelen;
                break;
        }

        if (len < sizeof(struct answer)) return -URTC_ERR_MALFORMED;
        struct answer *answer = (struct answer *)response;
        response += sizeof(struct answer);
        len -= sizeof(struct answer);

        size = ntohs(answer->size);
        if (len < size) return -URTC_ERR_MALFORMED;
        response += size;
        len -= size;
    }

    return 0;
}

/**
 * Check if run-length encoded query name resource records match hostname
 *
 * \param expected Expected hostname record.
 * \param q Length-encoded query names.
 * \param len Pointer to minimum length of q. Overwritten with actual length.
 *
 * \return Positive on match, 0 on no match, negative on malformed query.
 */
static int match(const char *expected, const uint8_t *q, int *len) {
    struct qname *qname;
    bool matches;
    int n_records;
    int n;

    if (!q) return -URTC_ERR_BAD_ARGUMENT;
    if (!expected) return -URTC_ERR_BAD_ARGUMENT;
    if (!len) return -URTC_ERR_BAD_ARGUMENT;

    n = *len;
    matches = true;

    n_records = 0;
    do {
        // minimum size check: a single root record (1 byte)
        if (n < 1) return -URTC_ERR_MALFORMED;
        qname = (struct qname *)(q);

        // minimum size check: length byte plus stated length
        if (n < 1 + qname->size) return -URTC_ERR_MALFORMED;

        // match records
        switch (n_records) {
            // match hostname record
            case 0:
                if (qname->size == strlen(expected)) {
                    if (0 == memcmp(qname->name, expected, qname->size)) {
                        break; // matched
                    }
                }
                matches = false;
                break;
            // match "local" record
            case 1:
                if (qname->size == sizeof("local")-1) {
                    if (0 == memcmp(qname->name, "local", sizeof("local")-1)) {
                        break; //matched
                    }
                }
                matches = false;
                break;
            // match root record
            case 2:
                if (qname->size == 0) {
                    break;
                }
                matches = false;
                break;
            // match ancillary records
            default:
                matches = false;
                break;
        }

        q += 1 + qname->size;
        n -= 1 + qname->size;
        n_records++;
    } while(qname->size);

    // update with actual length of run-length encoding
    *len -= n;

    return matches;
}

/**
 * Validate whether mDNS packet is a query for the specified hostname
 *
 * Only standard, single question queries are supported. All others
 * return an error.
 *
 * \param q Query buffer (from network)
 * \param n Size of query (in bytes)
 * \param hostname Queried hostname
 *
 * \return 0 on success, negative on error
 */
int mdns_validate_query(const uint8_t *q, size_t n, const char *hostname)
{
    const struct header *hdr = (struct header *)q;
    const uint8_t *qi = q;
    const size_t ni = n;
    int found;

    found = 0;

    // sanity checks
    if (!q) return -URTC_ERR_BAD_ARGUMENT;
    if (n < sizeof(struct header)) return -URTC_ERR_MALFORMED;

    q += sizeof(struct header);
    n -= sizeof(struct header);

    // transaction id must be zero
    // note: hdr in host byte order (0 is byte order agnostic)
    if (0 != hdr->id) return -URTC_ERR_MALFORMED;

    // only support standard, non-truncated, non-recursive queries
    // note: hdr in host byte order (non-0 is byte order agnostic)
    if (hdr->flags) return -URTC_ERR_NOT_IMPLEMENTED;

    // parse questions
    for (int j = 0; j < ntohs(hdr->n_question); j++) {
        bool matched;
        int ret;
        int len;

        // upper two bits set? "compressed" records
        matched = false;
        if (n > 2 && q[0] >> 6 == 3) {
            // compressed (i.e. offset to previous record)
            const uint16_t offset = ((q[0] & 0x3F) << 8) | q[1];
            if (offset > ni) return -URTC_ERR_MALFORMED;
            len = ni - offset;
            ret = match(hostname, qi + offset, &len);
            q += 2;
            n -= 2;
        } else {
            // uncompressed
            len = n;
            ret = match(hostname, q, &len);
            q += len;
            n -= len;
        }
        if (ret < 0) return ret;
        if (ret > 0) matched = true;

        // parse query type for A and AAAA records
        if (n < sizeof(uint16_t)) return -URTC_ERR_MALFORMED;
        if (matched) {
            uint16_t type = ntohs(*(uint16_t *)(q));
            if (TYPE_A == type || TYPE_AAAA == type) {
                found |= type;
            }
        }
        q += sizeof(uint16_t);
        n -= sizeof(uint16_t);

        // parse query class (ignore)
        if (n < sizeof(uint16_t)) return -URTC_ERR_MALFORMED;
        q += sizeof(uint16_t);
        n -= sizeof(uint16_t);
    }

    return found;
}

/**
 * Send mDNS response
 *
 * \param hostname NULL-terminated hostname (sans ".local" suffix)
 *
 * \return 0 on success, negative on error
 */
int mdns_send_response(int sockfd, const char *hostname) {
    struct ifaddrs *ifaddrs;
    struct header *hdr;
    uint8_t *wr;                        // writer (convenience pointer)
    uint8_t response[
        sizeof(struct header) +
        1 + HOST_NAME_MAX + 1 +         // max size of length and host record
        1 + sizeof(".local") +          // size of length and local record
        1 +                             // size of root record
        sizeof(struct answer) +
        16                              // size of AAAA record (largest record)
    ];
    hdr = (struct header *)(response);
    wr  = response;

    // write header
    *hdr = (struct header){
        .id           = 0,
        .flags        = htons(FLAG_RESPONSE),
        .n_question   = 0,
        .n_answer     = 0, 
        .n_authority  = 0,
        .n_additional = 0
    };
    wr += sizeof(struct header);

    // get linked list of network interfaces
    if (-1 == getifaddrs(&ifaddrs)) {
        urtc_log(URTC_ERROR, "%s", strerror(errno));
        goto _fail_getifaddrs;
    }

    // for each network interface...
    for (struct ifaddrs *ifaddr = ifaddrs; ifaddr; ifaddr = ifaddr->ifa_next) {
        // skip loopback interfaces
        if (ifaddr->ifa_flags & IFF_LOOPBACK) continue;

        // for an AF_INET* interface address, display the address
        if (ifaddr->ifa_addr && ifaddr->ifa_addr->sa_family == AF_INET) {

            // write host record
            {
                strncpy((char *)(wr + 1), hostname, HOST_NAME_MAX);
                wr[1 + HOST_NAME_MAX - 1] = '\0';
                *wr = strlen((char *)(wr + 1));
                wr += 1 + strlen((char *)(wr + 1));
            }

            // write local record
            {
                const char local[] = "local";
                *wr++ = sizeof(local) - 1;
                memcpy(wr, local, sizeof(local) - 1);
                wr += sizeof(local) - 1;
            }

            // write root record
            *wr++ = 0;

            // write answer
            {
                struct answer ans = {
                    .type  = htons(TYPE_A),
                    .class = htons(CACHE_FLUSH | CLASS_INTERNET),
                    .ttl   = htonl(DEFAULT_TTL),
                    .size  = htons(sizeof(struct in_addr)),
                };
                memcpy(wr, &ans, sizeof(ans));
                wr += sizeof(ans);

                memcpy(wr,
                    &(((struct sockaddr_in *)(ifaddr->ifa_addr))->sin_addr),
                    sizeof(struct in_addr));
                wr += sizeof(struct in_addr);
            }

            // increment number of answers
            hdr->n_answer++;

            if (hdr->n_answer == 1) {
                break;
            }
        }
    }

    // free linked list of network interfaces
    freeifaddrs(ifaddrs);

    // convert to network byte order
    hdr->n_answer = htons(hdr->n_answer);

    // send response
    struct sockaddr_in mgroup = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr(MDNS_ADDR),
        .sin_port = htons(MDNS_PORT)
    };
    if (-1 == sendto(
        sockfd,
        response,
        wr - response,
        0,
        (struct sockaddr *)(&mgroup),
        sizeof(mgroup)
    )) {
        urtc_log(URTC_FATAL, "sendto: %s", strerror(errno));
    }

    return 0;

_fail_getifaddrs:
    return -URTC_ERR;
}

/* vim: set expandtab ts=8 sw=4 tw=0 : */
