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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "log.h"
#include "sdp.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

/**
 * Parse group attribute
 *
 * Format is:
 *
 *     a=group:BUNDLE <mid> <mid> ...
 *
 * Only the BUNDLE semantics value is supported.
 *
 * \param sdp[out] SDP structure updated with bundle id tags.
 * \param value[in] NULL-terminated string containing SDP attribute line value
 *
 * \return 0 on success. Negative on error.
 */
static int sdp_parse_attr_group(struct sdp *sdp, char *val) {
	char *id = val;

	val = strsep(&id, " ");

	if (0 == strcmp("BUNDLE", val)) {
		for (int i = 0; id && i < SDP_MAX_BUNDLE_IDS; i++) {
			strncpy(sdp->mid[i], strsep(&id, " "), SDP_MAX_BUNDLE_ID_SIZE);
		}
	}
	return 0;
}

static int sdp_parse_attr_msid_semantic(struct sdp *sdp, const char *val) {
	/* not implemented */
	return 0;
}

static int sdp_parse_attr_rtcp(struct sdp *sdp, const char *val) {
	/* not implemented */
	return 0;
}

static int sdp_parse_attr_ice_ufrag(struct sdp *sdp, const char *val) {
	if (sdp->ufrag != strncpy(sdp->ufrag, val, sizeof(sdp->ufrag))) {
		return -1;
	}
	return 0;
}

static int sdp_parse_attr_ice_password(struct sdp *sdp, const char *val) {
	if (sdp->pwd != strncpy(sdp->pwd, val, sizeof(sdp->pwd))) {
		return -1;
	}
	return 0;
}

static int sdp_parse_attr_ice_options(struct sdp *sdp, const char *val) {
	if (0 == strcmp("trickle", val)) {
		sdp->ice_options.trickle = true;
	}
	return 0;
}

/**
 * Parse fingerprint
 *
 * Only SHA-256 fingerprint is supported.
 *
 * \param[out] sdp SDP structure updated with parsed content.
 * \param[in]  val NULL-terminated string.
 *
 * \return 0 on success. Negative on error.
 */
static int sdp_parse_attr_fingerprint(struct sdp *sdp, char *val) {
	char *fingerprint = val;

	val = strsep(&fingerprint, " ");

	if (fingerprint) {
		if (0 == strcmp("sha-256", val)) {
			if (32 != sscanf(fingerprint,
				"%2hhX:%2hhX:%2hhX:%2hhX:%2hhX:%2hhX:%2hhX:%2hhX:"
				"%2hhX:%2hhX:%2hhX:%2hhX:%2hhX:%2hhX:%2hhX:%2hhX:"
				"%2hhX:%2hhX:%2hhX:%2hhX:%2hhX:%2hhX:%2hhX:%2hhX:"
				"%2hhX:%2hhX:%2hhX:%2hhX:%2hhX:%2hhX:%2hhX:%2hhX",
				&sdp->fingerprint.sha256[0],  &sdp->fingerprint.sha256[1],
				&sdp->fingerprint.sha256[2],  &sdp->fingerprint.sha256[3],
				&sdp->fingerprint.sha256[4],  &sdp->fingerprint.sha256[5],
				&sdp->fingerprint.sha256[6],  &sdp->fingerprint.sha256[7],
				&sdp->fingerprint.sha256[8],  &sdp->fingerprint.sha256[9],
				&sdp->fingerprint.sha256[10], &sdp->fingerprint.sha256[11],
				&sdp->fingerprint.sha256[12], &sdp->fingerprint.sha256[13],
				&sdp->fingerprint.sha256[14], &sdp->fingerprint.sha256[15],
				&sdp->fingerprint.sha256[16], &sdp->fingerprint.sha256[17],
				&sdp->fingerprint.sha256[18], &sdp->fingerprint.sha256[19],
				&sdp->fingerprint.sha256[20], &sdp->fingerprint.sha256[21],
				&sdp->fingerprint.sha256[22], &sdp->fingerprint.sha256[23],
				&sdp->fingerprint.sha256[24], &sdp->fingerprint.sha256[25],
				&sdp->fingerprint.sha256[26], &sdp->fingerprint.sha256[27],
				&sdp->fingerprint.sha256[28], &sdp->fingerprint.sha256[29],
				&sdp->fingerprint.sha256[30], &sdp->fingerprint.sha256[31]
			)) {
				return -URTC_ERR_SDP_MALFORMED;
			}
		} else {
			return -URTC_ERR_SDP_UNSUPPORTED_FINGERPRINT_ALGO;
		}
	} else {
		return -URTC_ERR_SDP_MALFORMED;
	}

	return 0;
}

static int sdp_parse_attr_setup(struct sdp *sdp, const char *val) {
	/* not implemented */
	return 0;
}

static int sdp_parse_attr_mid(struct sdp *sdp, const char *val) {
	/* not implemented */
	return 0;
}

static int sdp_parse_attr_extmap(struct sdp *sdp, const char *val) {
	/* not implemented */
	return 0;
}

/**
 * Parse RTP map
 *
 * Expected format is:
 * 
 *     <payload type> <encoding name>/<clock rate> [/<encoding parameters>]
 *
 * \param[out] sdp SDP structure updated with parsed content.
 * \param[in]  val NULL-terminated string.
 *
 * \return 0 on success. Negative on error.
 */
static int sdp_parse_attr_rtpmap(struct sdp *sdp, char *val) {
	if (!val) return -URTC_ERR_SDP_MALFORMED;

	unsigned int pt;                    // payload type
	char         en[7];                 // encoding name
	unsigned int cr;                    // clock rate
	
	if (3 != sscanf(val, "%3d %6[a-zA-Z0-9]%*c%5d", &pt, en, &cr)) {
		return -URTC_ERR_SDP_MALFORMED_ATTRIBUTE;
	}

	if (0 == strcmp("H264", en)) {
		for (int i = 0; i < sdp->video.count; i++) {
			if (sdp->video.params[i].type == pt) {
				sdp->video.params[i].clock = cr;
				sdp->video.params[i].codec = SDP_CODEC_H264;
			}
		}
	}

	return 0;
}

static int sdp_parse_attr_rtcp_feedback(struct sdp *sdp, const char *val) {
	/* not implemented */
	return 0;
}

static int sdp_parse_attr_fmtp(struct sdp *sdp, const char *val) {
	/* not implemented */
	return 0;
}

/**
 * Parse SDP attribute line
 *
 * \param sdp[out] SDP structure updated with parsed content
 * \param attr[in] NULL-terminated SDP attribute string (after "a=")
 *
 * \return 0 on success. Negative on error.
 */
static int sdp_parse_attribute(struct sdp *sdp, char *attr) {
	char *value = attr;

	// Find colon separating attribute from value
	attr = strsep(&value, ":");

	if (value) {
		if (0 == strcmp("group", attr)) {
			return sdp_parse_attr_group(sdp, value);
		} else if (0 == strcmp("msid-semantic", attr)) {
			return sdp_parse_attr_msid_semantic(sdp, value);
		} else if (0 == strcmp("rtcp", attr)) {
			return sdp_parse_attr_rtcp(sdp, value);
		} else if (0 == strcmp("ice-ufrag", attr)) {
			return sdp_parse_attr_ice_ufrag(sdp, value);
		} else if (0 == strcmp("ice-pwd", attr)) {
			return sdp_parse_attr_ice_password(sdp, value);
		} else if (0 == strcmp("ice-options", attr)) {
			return sdp_parse_attr_ice_options(sdp, value);
		} else if (0 == strcmp("fingerprint", attr)) {
			return sdp_parse_attr_fingerprint(sdp, value);
		} else if (0 == strcmp("setup", attr)) {
			return sdp_parse_attr_setup(sdp, value);
		} else if (0 == strcmp("mid", attr)) {
			return sdp_parse_attr_mid(sdp, value);
		} else if (0 == strcmp("extmap", attr)) {
			return sdp_parse_attr_extmap(sdp, value);
		} else if (0 == strcmp("rtpmap", attr)) {
			return sdp_parse_attr_rtpmap(sdp, value);
		} else if (0 == strcmp("rtcp-fb", attr)) {
			return sdp_parse_attr_rtcp_feedback(sdp, value);
		} else if (0 == strcmp("fmtp", attr)) {
			return sdp_parse_attr_fmtp(sdp, value);
		}
	} else /* flag */ {
		if (0 == strcmp("recvonly", attr)) {
			sdp->mode = SDP_MODE_RECEIVE_ONLY;
		} else if (0 == strcmp("sendonly", attr)) {
			sdp->mode = SDP_MODE_SEND_ONLY;
		} else if (0 == strcmp("sendrecv", attr)) {
			sdp->mode = SDP_MODE_SEND_AND_RECEIVE;
		} else if (0 == strcmp("rtcp-mux", attr)) {
			sdp->rtcp_mux = true;
		} else if (0 == strcmp("rtcp-rsize", attr)) {
			sdp->rtcp_rsize = true;
		}
	}

	return 0;
}

/**
 * Parse media description line
 *
 * Format is:
 *
 *     m=<audio|video> <port> UDP/TLS/RTP/SAVPF <n> ...
 *
 * Only the UDP/TLS/RTP/SAVPF transport protocol is supported.
 *
 * \param sdp[out] SDP structure updated with username, session id and
 *                 session version.
 * \param value[in] NULL-terminated string containing SDP line value
 *
 * \return 0 on success. Negative on error.
 */
static int sdp_parse_media_description(struct sdp *sdp, char *value) {
	char *next = value;

	value = strsep(&next, " ");

	if (0 == strcmp("audio", value)) {
		/* not implemented */
	} else if (0 == strcmp("video", value)) {
		if (!next) return -URTC_ERR_SDP_MALFORMED_MEDIA;

		/* get port */
		value = strsep(&next, " ");
		if (1 != sscanf(value, "%5hd", &sdp->video.port)) {
			return -URTC_ERR_SDP_MALFORMED_MEDIA;
		}
		if (!next) return -URTC_ERR_SDP_MALFORMED_MEDIA;

		/* get protocol: only "UDP/TLS/RTP/SAVPF" supported */
		value = strsep(&next, " ");
		if (0 != strcmp("UDP/TLS/RTP/SAVPF", value)) {
			return -URTC_ERR_SDP_UNSUPPORTED_MEDIA_PROTOCOL;
		}
		if (!next) return -URTC_ERR_SDP_MALFORMED_MEDIA;

		/* get RTP payload types */
		sdp->video.count = 0;
		for (
			value = strsep(&next, " ");
			value && sdp->video.count < SDP_MAX_RTP_PAYLOAD_TYPES;
			value = strsep(&next, " ")
		) {
			unsigned int *v = &sdp->video.params[sdp->video.count].type;
			if (1 != sscanf(value, "%3d", v)) {
				return -URTC_ERR_SDP_MALFORMED_MEDIA;
			}
			sdp->video.count++;
		}

	} else if (0 == strcmp("text", value)) {
		/* ignore */
	} else if (0 == strcmp("message", value)) {
		/* ignore */
	} else if (0 == strcmp("application", value)) {
		/* ignore */
	} else {
		return -URTC_ERR_SDP_UNSUPPORTED_MEDIA_TYPE;
	}

	return 0;
}

static int sdp_parse_connection_data(struct sdp *sdp, const char *value) {
	return 0;
}

/**
 * Parse version line
 *
 * Format is:
 *
 *     v=0
 *
 * Only version zero is supported. Anything else is an error.
 *
 * \param value[in] NULL-terminated string containing SDP line value
 *
 * \return 0 on success. Negative on error.
 */
static int sdp_parse_version(struct sdp *sdp, const char *value) {
	if (0 != strcmp("0", value)) {
		return -URTC_ERR_SDP_MALFORMED_VERSION;
	}
	sdp->version = 0;

	return 0;
}

/**
 * Parse origin line
 *
 * Format is:
 *
 *     o=<username> <sess-id> <sess-version> <nettype> <addrtype>
 *       <unicast-address>
 *
 * <nettype> <addrtype> and <unicast-address> are ignored.
 *
 * \param sdp[out] SDP structure updated with username, session id and
 *                 session version.
 * \param value[in] NULL-terminated string containing SDP line value
 *
 * \return 0 on success. Negative on error.
 */
static int sdp_parse_origin(struct sdp *sdp, const char *value) {
	if (3 != sscanf(value,
		"%" TOSTRING(SDP_MAX_USERNAME_SIZE) "s "
		"%" TOSTRING(SDP_MAX_SESSION_ID_SIZE) "[0-9] "
		"%" TOSTRING(SDP_MAX_SESSION_VERSION_SIZE) "[0-9] "
		"IN IP4 127.0.0.1",
		sdp->username,
		sdp->session_id,
		sdp->session_version
	)) {
		return -URTC_ERR_SDP_MALFORMED_ORIGIN;
	}

	return 0;
}

static int sdp_parse_session_name(struct sdp *sdp, const char *value) {
	return 0;
}

/**
 * Parse timing line
 *
 * Format is:
 *
 *     t=<start-time> <stop-time>
 *
 * \param sdp[out] SDP structure updated with start and stop time
 * \param value[in] NULL-terminated string containing SDP line value
 *
 * \return 0 on success. Negative on error.
 */
static int sdp_parse_timing(struct sdp *sdp, const char *value) {
	if (2 != sscanf(value, "%10lli %10lli",
		(unsigned long long *)&sdp->start_time,
		(unsigned long long *)&sdp->stop_time
	)) {
		return -URTC_ERR_SDP_MALFORMED_TIMING;
	}

	return 0;
}

/**
 * Parse a line of SDP
 *
 * \param sdp[out] SDP structure to be updated with parsed information
 * \param line[in] NULL-terminated string containing one line of SDP
 *
 * \return 0 on success, negative on error.
 */
static int sdp_parse_line(struct sdp *sdp, char *line) {
	if (!line) return -URTC_ERR_BAD_ARGUMENT;
	if (0 == strlen(line)) return 0;
	if (strlen(line) < 3) return -URTC_ERR_SDP_MALFORMED;
	if ('=' != line[1]) return -URTC_ERR_SDP_MALFORMED;

	char type = line[0];
	char *value = line + 2;

	switch (type) {
	case 'a':
		return sdp_parse_attribute(sdp, value);
	case 'c':
		return sdp_parse_connection_data(sdp, value);
	case 'o':
		return sdp_parse_origin(sdp, value);
	case 'm':
		return sdp_parse_media_description(sdp, value);
	case 's':
		return sdp_parse_session_name(sdp, value);
	case 't':
		return sdp_parse_timing(sdp, value);
	case 'v':
		return sdp_parse_version(sdp, value);
	default:
		/* ignore unsupported types */
		return 0;
	}
}


int sdp_parse(struct sdp *dst, const char *str) {
	if (!str) return -URTC_ERR_BAD_ARGUMENT;
	if (!dst) return -URTC_ERR_BAD_ARGUMENT;

	err_t retval = 0;

	/* make a malleable copy of string for parser */
	char *copy = malloc(strlen(str));
	if (copy) {
		strcpy(copy, str);

		for (char *in = copy, *line = strsep(&in, "\r\n"); in; line = strsep(&in, "\r\n")) {
			retval = sdp_parse_line(dst, line);
			if (retval != 0) {
				log(ERROR, "sdp_parse_line returned %i for %s", retval, line);
				break;
			}
		}

		free(copy);
	} else {
		return -URTC_ERR_INSUFFICIENT_MEMORY;
	}

	return retval;
}

int sdp_serialize(char *dst, size_t len, const struct sdp *src) {
	int n;

	// sanity check
	if (!dst) return -URTC_ERR_BAD_ARGUMENT;
	if (!src) return -URTC_ERR_BAD_ARGUMENT;

	// write version
	n = snprintf(dst, len, "v=%u\n", src->version);
	if (n < 0) return -URTC_ERR_SDP_MALFORMED;
	if (n >= len) return -URTC_ERR_SDP_MALFORMED;
	dst += n;
	len -= n;

	// write originator and session identifier
	n = snprintf(dst, len,
		"o=liburtc/0.0.0 %s %s IN IP4 127.0.0.1\n",
		src->session_id,
		src->session_version
	);
	if (n < 0) return -URTC_ERR_SDP_MALFORMED;
	if (n >= len) return -URTC_ERR_SDP_MALFORMED;
	dst += n;
	len -= n;

	// write session name
	n = snprintf(dst, len, "s=%s\n",
		0 == strlen(src->session_name) ? " " : src->session_name);
	if (n < 0) return -URTC_ERR_SDP_MALFORMED;
	if (n >= len) return -URTC_ERR_SDP_MALFORMED;
	dst += n;
	len -= n;

	// write uri
	n = snprintf(dst, len, "u=http://www.liburtc.org\n");
	if (n < 0) return -URTC_ERR_SDP_MALFORMED;
	if (n >= len) return -URTC_ERR_SDP_MALFORMED;
	dst += n;
	len -= n;

	// write time descriptor
	n = snprintf(dst, len, "t=%" PRIu64 " %" PRIu64 "\n",
		src->start_time, src->stop_time);
	if (n < 0) return -URTC_ERR_SDP_MALFORMED;
	if (n >= len) return -URTC_ERR_SDP_MALFORMED;
	dst += n;
	len -= n;

	// write session attribute
	n = snprintf(dst, len, "a=group:BUNDLE");
	if (n < 0) return -URTC_ERR_SDP_MALFORMED;
	if (n >= len) return -URTC_ERR_SDP_MALFORMED;
	dst += n;
	len -= n;
	for (int i = 0; i < SDP_MAX_BUNDLE_IDS; i++) {
		if (strlen(src->mid[i]) > 0) {
			n = snprintf(dst, len, " %s", src->mid[i]);
			if (n < 0) return -URTC_ERR_SDP_MALFORMED;
			if (n >= len) return -URTC_ERR_SDP_MALFORMED;
			dst += n;
			len -= n;
		}
	}
	n = snprintf(dst, len, "\n");
	if (n < 0) return -URTC_ERR_SDP_MALFORMED;
	if (n >= len) return -URTC_ERR_SDP_MALFORMED;
	dst += n;
	len -= n;

	// write video media description
	if (src->video.count > 0) {
		n = snprintf(
			dst, len, "m=video %hd UDP/TLS/RTP/SAVPF", src->video.port
		);
		if (n < 0) return -URTC_ERR_SDP_MALFORMED;
		if (n >= len) return -URTC_ERR_SDP_MALFORMED;
		dst += n;
		len -= n;
		for (int i = 0; i < src->video.count; i++) {
			if (src->video.params[i].codec == SDP_CODEC_H264) {
				n = snprintf(dst, len, " %d", src->video.params[i].type);
				if (n < 0) return -URTC_ERR_SDP_MALFORMED;
				if (n > len) return -URTC_ERR_SDP_MALFORMED;
				dst += n;
				len -= n;
			}
		}
		n = snprintf(dst, len, "\n");
		if (n < 0) return -URTC_ERR_SDP_MALFORMED;
		if (n >= len) return -URTC_ERR_SDP_MALFORMED;
		dst += n;
		len -= n;
	}

	// TODO write rtcp attribute

	// write connection information (TODO IPv6)
	n = snprintf(dst, len, "c=IN IP4 0.0.0.0\n");
	if (n < 0) return -URTC_ERR_SDP_MALFORMED;
	if (n >= len) return -URTC_ERR_SDP_MALFORMED;
	dst += n;
	len -= n;

	// write media attribute: ice ufrag
	n = snprintf(dst, len, "a=ice-ufrag:%s\n", src->ufrag);
	if (n < 0) return -URTC_ERR_SDP_MALFORMED;
	if (n >= len) return -URTC_ERR_SDP_MALFORMED;
	dst += n;
	len -= n;

	// write media attribute: ice pwd
	n = snprintf(dst, len, "a=ice-pwd:%s\n", src->pwd);
	if (n < 0) return -URTC_ERR_SDP_MALFORMED;
	if (n >= len) return -URTC_ERR_SDP_MALFORMED;
	dst += n;
	len -= n;

	// write media attribute: trickle ICE
	if (src->ice_options.trickle) {
		n = snprintf(dst, len, "a=ice-options:trickle\n");
		if (n < 0) return -URTC_ERR_SDP_MALFORMED;
		if (n >= len) return -URTC_ERR_SDP_MALFORMED;
		dst += n;
		len -= n;
	}

	// write media attribute: fingerprint
	n = snprintf(dst, len,
		"a=fingerprint:sha-256 "
		"%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:"
		"%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:"
		"%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:"
		"%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX\n",
		src->fingerprint.sha256[0], src->fingerprint.sha256[1],
		src->fingerprint.sha256[2], src->fingerprint.sha256[3],
		src->fingerprint.sha256[4], src->fingerprint.sha256[5],
		src->fingerprint.sha256[6], src->fingerprint.sha256[7],
		src->fingerprint.sha256[8], src->fingerprint.sha256[9],
		src->fingerprint.sha256[10], src->fingerprint.sha256[11],
		src->fingerprint.sha256[12], src->fingerprint.sha256[13],
		src->fingerprint.sha256[14], src->fingerprint.sha256[15],
		src->fingerprint.sha256[16], src->fingerprint.sha256[17],
		src->fingerprint.sha256[18], src->fingerprint.sha256[19],
		src->fingerprint.sha256[20], src->fingerprint.sha256[21],
		src->fingerprint.sha256[22], src->fingerprint.sha256[23],
		src->fingerprint.sha256[24], src->fingerprint.sha256[25],
		src->fingerprint.sha256[26], src->fingerprint.sha256[27],
		src->fingerprint.sha256[28], src->fingerprint.sha256[29],
		src->fingerprint.sha256[30], src->fingerprint.sha256[31]
	);
	if (n < 0) return -URTC_ERR_SDP_MALFORMED;
	if (n >= len) return -URTC_ERR_SDP_MALFORMED;
	dst += n;
	len -= n;

	// TODO write setup, mid

	// write media attribute: mode
	switch (src->mode) {
		case SDP_MODE_RECEIVE_ONLY:
			n = snprintf(dst, len, "a=recvonly\n");
			break;
		case SDP_MODE_SEND_ONLY:
			n = snprintf(dst, len, "a=sendonly\n");
			break;
		case SDP_MODE_SEND_AND_RECEIVE:
			n = snprintf(dst, len, "a=sendrecv\n");
			break;
		default:
			return -URTC_ERR_SDP_MALFORMED;
			break;
	}
	if (n < 0) return -URTC_ERR_SDP_MALFORMED;
	if (n >= len) return -URTC_ERR_SDP_MALFORMED;
	dst += n;
	len -= n;

	// write media attribute: rtcp mux
	if (src->rtcp_mux) {
		n = snprintf(dst, len, "a=rtcp-mux\n");
		if (n < 0) return -URTC_ERR_SDP_MALFORMED;
		if (n >= len) return -URTC_ERR_SDP_MALFORMED;
		dst += n;
		len -= n;
	}

	// write media attribute: rtcp rsize
	if (src->rtcp_rsize) {
		n = snprintf(dst, len, "a=rtcp-rsize\n");
		if (n < 0) return -URTC_ERR_SDP_MALFORMED;
		if (n >= len) return -URTC_ERR_SDP_MALFORMED;
		dst += n;
		n -= n;
	}

	// write dynamic profiles
	for (int i = 0; i < src->video.count; i++) {
		if (src->video.params[i].codec == SDP_CODEC_H264) {
			n = snprintf(dst, len, "a=rtpmap:%d H264/%d\n",
				src->video.params[i].type,
				src->video.params[i].clock
			);
			if (n < 0) return -URTC_ERR_SDP_MALFORMED;
			if (n >= len) return -URTC_ERR_SDP_MALFORMED;
			dst += n;
			len -= n;
		}
	}

	// write media attribute: media id
	// TODO fix hardcoded media id
	n = snprintf(dst, len, "a=mid:0\n");
	if (n < 0) return -URTC_ERR_SDP_MALFORMED;
	if (n >= len) return -URTC_ERR_SDP_MALFORMED;
	dst += n;
	len -= n;

	return 0;
}
