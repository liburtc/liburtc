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

#ifndef URTC_G711_H
#define URTC_G711_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

extern const uint8_t pcm2ulaw[];
extern const int16_t ulaw2pcm[];

/**
 * ITU-T G.711 compand a 16-bit signed PCM audio buffer. Buffer may contain
 * multiple channels, in which case \a n must be the number of frames times
 * the number of channels.
 *
 * Only the upper 13-bits are companded. The lowest 3-bits are discarded.
 *
 * \a dst must be allocated and point to at least (\a n / 2) bytes. \a dst
 * and \a src may overlap.
 * 
 * @param dst Destination buffer
 * @param src Signed 16-bit sample source buffer
 * @param n   Number of samples (frames times channels)
 */
void g711_encode(uint8_t *dst, const int16_t *src, size_t n);

/**
 * ITU-T G.711 decompand to a 16-bit signed PCM audio buffer. Buffer may
 * contain multiple channels, in which case \a n must be the number of frames
 * times the number of channels.
 *
 * The decompanded samples are normalized to 16-bits (the lowest 3-bits will be
 * zero).
 *
 * \a dst must NOT overlap with \a src.
 *
 * @param dst Destination buffer
 * @param src Signed 16-bit sample source buffer
 * @param n   Number of samples (frames times channels)
 */
void g711_decode(int16_t *dst, const uint8_t *src, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* URTC_G711_H */

/* vim: set expandtab ts=8 sw=4 tw=0 : */
