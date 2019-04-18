/**
 * liburtc
 * Copyright 2020 Chris Hiszpanski
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef _URTC_MDNS_H
#define _URTC_MDNS_H

#include <stddef.h>
#include <stdint.h>

#define TYPE_A                       1  // IPv4 record
#define TYPE_AAAA                   28  // IPv6 record

// server: for processing queries and generating responses
int mdns_subscribe();
int mdns_unsubscribe(int sockfd);
int mdns_validate_query(const uint8_t *q, size_t n, const char *hostname);
int mdns_send_response(int sockfd, const char *hostname);

// client: for sending queries and processing responses
int mdns_parse_response(const void *response, size_t len);
int mdns_query(const char *name);

#endif // _URTC_MDNS_H
