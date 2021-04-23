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

#include <stdio.h>

#include "g711.h"

void g711_encode(uint8_t *dst, const int16_t *src, size_t n) {
    int i;

    for (i = 0; i < n; i++) {
        dst[i] = pcm2ulaw[0x1FFF & (src[i] >> 3)];
    }
}

void g711_decode(int16_t *dst, const uint8_t *src, size_t n) {
    int i;

    for (i = 0; i < n; i++) {
        fprintf(stderr, "i = %i, src[i] = %i\n", i, src[i]);
        fprintf(stderr, "ulaw2pcm[i] = %i\n", ulaw2pcm[0]);
//      dst[i] = ulaw2pcm[src[i]] << 3;
        dst[i] = 0;
    }
}

/* vim: set expandtab ts=8 sw=4 tw=0 : */
