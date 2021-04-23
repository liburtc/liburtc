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

/**
 * Pseudo-random univerally-unique identifier generator [^RFC4122].
 *
 * [^RFC4122]: https://tools.ietf.org/html/rfc4122#section-4.4
 */

#include <stdio.h>
#include <stdlib.h>

#include "prng.h"
#include "uuid.h"

/**
 * Generate a pseudo-random UUID.
 *
 * \return Populated UUID.
 */
struct uuid uuid_create() {
    struct uuid u;

    // fill with randomness
    prng(&u, sizeof(u));

    // set reserved and version bits
    u.clk_seq_hi_and_reserved = (u.clk_seq_hi_and_reserved & 0x3F) | 0x80;
    u.time_hi_and_version = (u.time_hi_and_version & 0x0FFF) | 0x4000;

    return u;
}

/**
 * Generate pseudorandom UUID (as a string)
 *
 * \param s Destination string memory.
 *
 */
void uuid_create_str(char s[UUID_STR_LEN]) {
    struct uuid u = uuid_create();

    sprintf(s, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        u.time_low,
        u.time_mid,
        u.time_hi_and_version,
        u.clk_seq_hi_and_reserved,
        u.clk_seq_low,
        u.node[0],
        u.node[1],
        u.node[2],
        u.node[3],
        u.node[4],
        u.node[5]
    );
}

/* vim: set expandtab ts=8 sw=4 tw=0 : */
