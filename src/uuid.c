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
