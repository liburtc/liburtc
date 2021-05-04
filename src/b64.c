/**
 * Copyright (c) 2021 Chris Hiszpanski. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author(s) may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "b64.h"

void b64_encode(char *dst, const char *src, size_t size) {
    const char lut[64] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    char spare = 0;
    char state = 0;

    while (size || spare) {
        switch (state) {
        case 0:
            *dst++ = lut[((*src) >> 2) & 0x3F];
            spare = (*src) & 0x3;
            state = 1;
            src++;
            size--;
            break;
        case 1:
            *dst++ = lut[(spare << 4) | (((*src) >> 4) & 0x0F)];
            spare = (*src) & 0x0F;
            state = 2;
            src++;
            size--;
            break;
        case 2:
            *dst++ = lut[(spare << 2) | (((*src) >> 6) & 0x03)];
            spare = (*src) & 0x3F;
            state = 3;
            size--;
            break;
        case 3:
            *dst++ = lut[spare];
            spare = 0;
            state = 0;
            break;
        }
    }
    *dst = '\0';
}

