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

#include <assert.h>
#include <stdint.h>

#include "mdns.h"

const uint8_t response[] = {
	// header
	0x00, 0x00, // transaction id
	0x84, 0x00, // flags
	0x00, 0x01, // questions
	0x00, 0x02, // answer RRs
	0x00, 0x00, // authority RRs
	0x00, 0x00, // additional RRs

	// query
	0x07, 0x6c, 0x69, 0x62, 0x75, 0x72, 0x74, 0x63, // "liburtc"
	0x05, 0x6c, 0x6f, 0x63, 0x61, 0x6c, // "local"
	0x00, // root record
	0x00, 0xff, // type: any
	0x80, 0x01, // class: unicast | in(ternet)

	// answer
	0xc0, 0x0c, // name
	0x00, 0x1c, // type: AAAA
	0x00, 0x01, // class: in(ternet)
	0x00, 0x00, 0x00, 0x0a, // ttl
	0x00, 0x10, // size
	0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // IPv6
	0x72, 0x85, 0xc2, 0xff, 0xfe, 0x07, 0x1f, 0x03, // IPv6 (continued)

	// answer
	0xc0, 0x0c, // name
	0x00, 0x01, // type: A
	0x00, 0x01, // class: in(ternet)
	0x00, 0x00, 0x00, 0x0a, // ttl
	0x00, 0x04, // size
	0xc0, 0xa8, 0x01, 0x64 // IPv4
};

int main(int argc, char **argv) {
	assert(0 == mdns_parse_response(response, sizeof(response)));
	return 0;
}
