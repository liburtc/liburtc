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

#ifndef _URTC_SDP_H
#define _URTC_SDP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>


// Maximum size of SDP string (in bytes)
#define SDP_MAX_SIZE                    2048

#define SDP_MAX_USERNAME_SIZE           32
#define SDP_MAX_SESSION_ID_SIZE         32
#define SDP_MAX_SESSION_VERSION_SIZE    32
#define SDP_MAX_SESSION_NAME_SIZE       32

#define SDP_MAX_BUNDLE_IDS              5
#define SDP_MAX_BUNDLE_ID_SIZE          32
#define SDP_MAX_RTP_PAYLOAD_TYPES       32


typedef enum sdp_mode {
    SDP_MODE_SEND_AND_RECEIVE = 0,
    SDP_MODE_RECEIVE_ONLY,
    SDP_MODE_SEND_ONLY
} sdp_mode_t;

typedef enum sdp_media {
    SDP_MEDIA_TYPE_NULL = 0,
    SDP_MEDIA_TYPE_AUDIO,
    SDP_MEDIA_TYPE_VIDEO,
    SDP_MEDIA_TYPE_TEXT,
    SDP_MEDIA_TYPE_APPLICATION,
    SDP_MEDIA_TYPE_MESSAGE
} sdp_media_type_t;

// Codecs recognized by SDP parser
typedef enum sdp_codec {
    SDP_CODEC_NULL = 0,
    SDP_CODEC_H264,
    SDP_CODEC_VP9
} sdp_codec_t;

// Dynamic payload type
typedef struct sdp_rtpmap {
    unsigned int      type;             // Dynamic RTP payload type (e.g. 96)
    enum sdp_codec    codec;            // H264, VP9, etc.
    unsigned int      clock;            // Typically 90kHz for video
    unsigned int      flags;
} sdp_rtpmap_t;

typedef struct sdp {
    // protocol version
    uint8_t version;

    // originator and session identifier
    char username[SDP_MAX_USERNAME_SIZE+1];
    char session_id[SDP_MAX_SESSION_ID_SIZE+1];
    char session_version[SDP_MAX_SESSION_VERSION_SIZE+1];

    // session name
    char session_name[SDP_MAX_SESSION_NAME_SIZE+1];

    // time description (mandatory)
    uint64_t start_time;
    uint64_t stop_time;

    // session attributes
    
    // bundle media identification tags
    char mid[SDP_MAX_BUNDLE_IDS][SDP_MAX_BUNDLE_ID_SIZE+1];
    
    // ice
    char ufrag[4*256];                  // ice-ufrag (256 unicode chars max.)
    char pwd[4*256];                    // ice-pwd (256 unicode chars max.)
    struct {
        bool trickle;
    } ice_options;

    // video media
    struct {
        uint16_t port;
        struct sdp_rtpmap params[SDP_MAX_RTP_PAYLOAD_TYPES];
        int count;
    } video;

    // audio media
    struct {
        uint16_t port;
        struct sdp_rtpmap params[SDP_MAX_RTP_PAYLOAD_TYPES];
        int count;
    } audio;

    sdp_mode_t mode;                    // send, receive, and send-and-receive

    bool rtcp_mux;                      // is rtcp multiplexed on same socket
    bool rtcp_rsize;

    union {
        uint8_t sha256[32];             // certificate fingerprint
    } fingerprint;

} sdp_t;


/**
 * Parse SDP string into structure
 *
 * \param dst[out] SDP structure
 * \param src[in] NULL-terminated SDP string
 *
 * \return 0 on success, negative on error.
 */
int sdp_parse(struct sdp *dst, const char *src);

/**
 * Serialize SDP structure into string
 *
 * \param dst[out] NULL-terminated SDP string
 * \param len[in] Capacity of dst
 * \param src[in] SDP structure
 *
 * \return 0 on success, negative on error.
 */
int sdp_serialize(char *dst, size_t len, const struct sdp *src);

#ifdef __cplusplus
}
#endif

#endif /* _URTC_SDP_H */

/* vim: set expandtab ts=8 sw=4 tw=0 : */
