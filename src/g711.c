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
//		dst[i] = ulaw2pcm[src[i]] << 3;
		dst[i] = 0;
	}
}
