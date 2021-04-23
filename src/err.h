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

/* vim: set expandtab ts=8 sw=4 tw=0 : */
