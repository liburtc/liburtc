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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "g711.h"


int main(int argc, char **argv) {

	{
		const int16_t src[] = {
			0,
			4096,
			8192,
			-8192,
			-4096
		};
		uint8_t dst[sizeof(src) / sizeof(int16_t)];
		g711_encode(dst, src, sizeof(src) / sizeof(int16_t));
	}

	{
		const uint8_t src[] = {
			0x00,
			0x7f,
			0x80,
			0xff,
		};
		int16_t dst[sizeof(src) / sizeof(uint8_t)];
		g711_decode(dst, src, sizeof(src) / sizeof(uint8_t));
	}

	return 0;
}
