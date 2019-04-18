/*
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

#ifndef _URTC_ERR_H
#define _URTC_ERR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum err {
	URTC_SUCCESS = 0,

	URTC_ERR,

	URTC_ERR_BAD_ARGUMENT,
	URTC_ERR_INSUFFICIENT_MEMORY,
	URTC_ERR_MALFORMED,
	URTC_ERR_NOT_IMPLEMENTED,

	URTC_ERR_PEERCONN_MISSING_REMOTE_DESC,

	URTC_ERR_SDP_MALFORMED,
	URTC_ERR_SDP_MALFORMED_VERSION,
	URTC_ERR_SDP_MALFORMED_ORIGIN,
	URTC_ERR_SDP_MALFORMED_TIMING,
	URTC_ERR_SDP_MALFORMED_MEDIA,
	URTC_ERR_SDP_MALFORMED_ATTRIBUTE,
	URTC_ERR_SDP_UNSUPPORTED_FINGERPRINT_ALGO,
	URTC_ERR_SDP_UNSUPPORTED_MEDIA_PROTOCOL,
	URTC_ERR_SDP_UNSUPPORTED_MEDIA_TYPE
} err_t;

#ifdef __cplusplus
}
#endif

#endif /* _URTC_ERR_H */
