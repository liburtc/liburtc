/*
 * liburtc
 * Copyright (C) 2019 Chris Hiszpanski
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
