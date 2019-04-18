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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "sdp.h"

// Chrome 73.0.3683.103 (Official Build) (64-bit)
const char *chrome =
	"v=0\n"
	"o=- 2136573259711410686 2 IN IP4 127.0.0.1\n"
	"s=-\n"
	"t=0 0\n"
	"a=group:BUNDLE 0\n"
	"a=msid-semantic: WMS\n"
	"m=video 9 UDP/TLS/RTP/SAVPF 96 97 98 99 100 101 102 122 127 121 125 107 108 109 124 120 123 119 114 115 116\n"
	"c=IN IP4 0.0.0.0\n"
	"a=rtcp:9 IN IP4 0.0.0.0\n"
	"a=ice-ufrag:DPkQ\n"
	"a=ice-pwd:23oU5vsiyBKLHbND/Ql8f7gZ\n"
	"a=ice-options:trickle\n"
	"a=fingerprint:sha-256 D0:44:DF:68:71:39:56:0B:D3:61:7A:F2:42:5B:1B:0A:CD:B2:72:84:3A:DE:0F:22:CA:8C:B0:06:0A:8D:A2:00\n"
	"a=setup:actpass\n"
	"a=mid:0\n"
	"a=extmap:2 urn:ietf:params:rtp-hdrext:toffset\n"
	"a=extmap:3 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\n"
	"a=extmap:4 urn:3gpp:video-orientation\n"
	"a=extmap:5 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\n"
	"a=extmap:6 http://www.webrtc.org/experiments/rtp-hdrext/playout-delay\n"
	"a=extmap:7 http://www.webrtc.org/experiments/rtp-hdrext/video-content-type\n"
	"a=extmap:8 http://www.webrtc.org/experiments/rtp-hdrext/video-timing\n"
	"a=extmap:10 http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07\n"
	"a=extmap:12 http://www.webrtc.org/experiments/rtp-hdrext/color-space\n"
	"a=extmap:9 urn:ietf:params:rtp-hdrext:sdes:mid\n"
	"a=extmap:13 urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id\n"
	"a=extmap:14 urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id\n"
	"a=recvonly\n"
	"a=rtcp-mux\n"
	"a=rtcp-rsize\n"
	"a=rtpmap:96 VP8/90000\n"
	"a=rtcp-fb:96 goog-remb\n"
	"a=rtcp-fb:96 transport-cc\n"
	"a=rtcp-fb:96 ccm fir\n"
	"a=rtcp-fb:96 nack\n"
	"a=rtcp-fb:96 nack pli\n"
	"a=rtpmap:97 rtx/90000\n"
	"a=fmtp:97 apt=96\n"
	"a=rtpmap:98 VP9/90000\n"
	"a=rtcp-fb:98 goog-remb\n"
	"a=rtcp-fb:98 transport-cc\n"
	"a=rtcp-fb:98 ccm fir\n"
	"a=rtcp-fb:98 nack\n"
	"a=rtcp-fb:98 nack pli\n"
	"a=fmtp:98 profile-id=0\n"
	"a=rtpmap:99 rtx/90000\n"
	"a=fmtp:99 apt=98\n"
	"a=rtpmap:100 VP9/90000\n"
	"a=rtcp-fb:100 goog-remb\n"
	"a=rtcp-fb:100 transport-cc\n"
	"a=rtcp-fb:100 ccm fir\n"
	"a=rtcp-fb:100 nack\n"
	"a=rtcp-fb:100 nack pli\n"
	"a=fmtp:100 profile-id=2\n"
	"a=rtpmap:101 rtx/90000\n"
	"a=fmtp:101 apt=100\n"
	"a=rtpmap:102 H264/90000\n"
	"a=rtcp-fb:102 goog-remb\n"
	"a=rtcp-fb:102 transport-cc\n"
	"a=rtcp-fb:102 ccm fir\n"
	"a=rtcp-fb:102 nack\n"
	"a=rtcp-fb:102 nack pli\n"
	"a=fmtp:102 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42001f\n"
	"a=rtpmap:122 rtx/90000\n"
	"a=fmtp:122 apt=102\n"
	"a=rtpmap:127 H264/90000\n"
	"a=rtcp-fb:127 goog-remb\n"
	"a=rtcp-fb:127 transport-cc\n"
	"a=rtcp-fb:127 ccm fir\n"
	"a=rtcp-fb:127 nack\n"
	"a=rtcp-fb:127 nack pli\n"
	"a=fmtp:127 level-asymmetry-allowed=1;packetization-mode=0;profile-level-id=42001f\n"
	"a=rtpmap:121 rtx/90000\n"
	"a=fmtp:121 apt=127\n"
	"a=rtpmap:125 H264/90000\n"
	"a=rtcp-fb:125 goog-remb\n"
	"a=rtcp-fb:125 transport-cc\n"
	"a=rtcp-fb:125 ccm fir\n"
	"a=rtcp-fb:125 nack\n"
	"a=rtcp-fb:125 nack pli\n"
	"a=fmtp:125 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f\n"
	"a=rtpmap:107 rtx/90000\n"
	"a=fmtp:107 apt=125\n"
	"a=rtpmap:108 H264/90000\n"
	"a=rtcp-fb:108 goog-remb\n"
	"a=rtcp-fb:108 transport-cc\n"
	"a=rtcp-fb:108 ccm fir\n"
	"a=rtcp-fb:108 nack\n"
	"a=rtcp-fb:108 nack pli\n"
	"a=fmtp:108 level-asymmetry-allowed=1;packetization-mode=0;profile-level-id=42e01f\n"
	"a=rtpmap:109 rtx/90000\n"
	"a=fmtp:109 apt=108\n"
	"a=rtpmap:124 H264/90000\n"
	"a=rtcp-fb:124 goog-remb\n"
	"a=rtcp-fb:124 transport-cc\n"
	"a=rtcp-fb:124 ccm fir\n"
	"a=rtcp-fb:124 nack\n"
	"a=rtcp-fb:124 nack pli\n"
	"a=fmtp:124 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=4d0032\n"
	"a=rtpmap:120 rtx/90000\n"
	"a=fmtp:120 apt=124\n"
	"a=rtpmap:123 H264/90000\n"
	"a=rtcp-fb:123 goog-remb\n"
	"a=rtcp-fb:123 transport-cc\n"
	"a=rtcp-fb:123 ccm fir\n"
	"a=rtcp-fb:123 nack\n"
	"a=rtcp-fb:123 nack pli\n"
	"a=fmtp:123 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=640032\n"
	"a=rtpmap:119 rtx/90000\n"
	"a=fmtp:119 apt=123\n"
	"a=rtpmap:114 red/90000\n"
	"a=rtpmap:115 rtx/90000\n"
	"a=fmtp:115 apt=114\n"
	"a=rtpmap:116 ulpfec/90000\n";

// Safari 13.1 (14609.1.20.111.8)
const char *safari =
	"v=0\n"
	"o=- 3389190485417077944 2 IN IP4 127.0.0.1\n"
	"s=-\n"
	"t=0 0\n"
	"a=group:BUNDLE 0\n"
	"a=msid-semantic: WMS\n"
	"m=video 9 UDP/TLS/RTP/SAVPF 96 97 98 99 100 101 127 125 104\n"
	"c=IN IP4 0.0.0.0\n"
	"a=rtcp:9 IN IP4 0.0.0.0\n"
	"a=ice-ufrag:yMtQ\n"
	"a=ice-pwd:92GWQlqPVFfVjlxV2qSlQxEq\n"
	"a=ice-options:trickle\n"
	"a=fingerprint:sha-256 D7:41:A3:34:FC:54:27:FD:D1:2A:58:1D:9E:01:8A:C8:A9:F3:E0:BE:66:B3:D9:58:FC:7D:59:A7:BA:D6:99:F3\n"
	"a=setup:actpass\n"
	"a=mid:0\n"
	"a=extmap:14 urn:ietf:params:rtp-hdrext:toffset\n"
	"a=extmap:2 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\n"
	"a=extmap:13 urn:3gpp:video-orientation\n"
	"a=extmap:3 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\n"
	"a=extmap:12 http://www.webrtc.org/experiments/rtp-hdrext/playout-delay\n"
	"a=extmap:11 http://www.webrtc.org/experiments/rtp-hdrext/video-content-type\n"
	"a=extmap:7 http://www.webrtc.org/experiments/rtp-hdrext/video-timing\n"
	"a=extmap:8 http://tools.ietf.org/html/draft-ietf-avtext-framemarking-07\n"
	"a=extmap:9 http://www.webrtc.org/experiments/rtp-hdrext/color-space\n"
	"a=extmap:4 urn:ietf:params:rtp-hdrext:sdes:mid\n"
	"a=extmap:5 urn:ietf:params:rtp-hdrext:sdes:rtp-stream-id\n"
	"a=extmap:6 urn:ietf:params:rtp-hdrext:sdes:repaired-rtp-stream-id\n"
	"a=sendrecv\n"
	"a=msid:- d770ebc2-b725-4de0-8314-a76a8a67695e\n"
	"a=rtcp-mux\n"
	"a=rtcp-rsize\n"
	"a=rtpmap:96 H264/90000\n"
	"a=rtcp-fb:96 goog-remb\n"
	"a=rtcp-fb:96 transport-cc\n"
	"a=rtcp-fb:96 ccm fir\n"
	"a=rtcp-fb:96 nack\n"
	"a=rtcp-fb:96 nack pli\n"
	"a=fmtp:96 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=640c1f\n"
	"a=rtpmap:97 rtx/90000\n"
	"a=fmtp:97 apt=96\n"
	"a=rtpmap:98 H264/90000\n"
	"a=rtcp-fb:98 goog-remb\n"
	"a=rtcp-fb:98 transport-cc\n"
	"a=rtcp-fb:98 ccm fir\n"
	"a=rtcp-fb:98 nack\n"
	"a=rtcp-fb:98 nack pli\n"
	"a=fmtp:98 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f\n"
	"a=rtpmap:99 rtx/90000\n"
	"a=fmtp:99 apt=98\n"
	"a=rtpmap:100 VP8/90000\n"
	"a=rtcp-fb:100 goog-remb\n"
	"a=rtcp-fb:100 transport-cc\n"
	"a=rtcp-fb:100 ccm fir\n"
	"a=rtcp-fb:100 nack\n"
	"a=rtcp-fb:100 nack pli\n"
	"a=rtpmap:101 rtx/90000\n"
	"a=fmtp:101 apt=100\n"
	"a=rtpmap:127 red/90000\n"
	"a=rtpmap:125 rtx/90000\n"
	"a=fmtp:125 apt=127\n"
	"a=rtpmap:104 ulpfec/90000\n"
	"a=ssrc-group:FID 737915040 1139142965\n"
	"a=ssrc:737915040 cname:uGvMyunFXUYJjCuf\n"
	"a=ssrc:737915040 msid:- d770ebc2-b725-4de0-8314-a76a8a67695e\n"
	"a=ssrc:737915040 mslabel:-\n"
	"a=ssrc:737915040 label:d770ebc2-b725-4de0-8314-a76a8a67695e\n"
	"a=ssrc:1139142965 cname:uGvMyunFXUYJjCuf\n"
	"a=ssrc:1139142965 msid:- d770ebc2-b725-4de0-8314-a76a8a67695e\n"
	"a=ssrc:1139142965 mslabel:-\n"
	"a=ssrc:1139142965 label:d770ebc2-b725-4de0-8314-a76a8a67695e\n";

int main(int argc, char **argv) {

	// Test SDP deserialize
	{
		sdp_t sdp;
		assert(-URTC_ERR_BAD_ARGUMENT == sdp_parse(&sdp, NULL));
		assert(0 == sdp_parse(&sdp, ""));
	}

	{
		sdp_t sdp;
		assert(0 == sdp_parse(&sdp, chrome));
		assert(0 == sdp.version);
		assert(0 == sdp.start_time);
		assert(0 == sdp.stop_time);

	}

	// Test parser against Chrome offer
	{
		sdp_t sdp;
		assert(0 == sdp_parse(&sdp, chrome));
		assert(0 == sdp.version);
		assert(0 == strcmp("-", sdp.username));
		assert(0 == strcmp("2136573259711410686", sdp.session_id));
		assert(0 == strcmp("2", sdp.session_version));
		assert(0 == sdp.start_time);
		assert(0 == sdp.stop_time);
		{
			assert(21 == sdp.video.count);
			assert(9 == sdp.video.port);
			unsigned int expected_type[] = {
				96,  97,  98,  99,  100, 101, 102, 122, 127, 121,
				125, 107, 108, 109, 124, 120, 123, 119, 114, 115,
				116
			};
			for (int i = 0; i < sdp.video.count; i++) {
				assert(expected_type[i] == sdp.video.params[i].type);
			}
		}
		assert(0 == strcmp("DPkQ", sdp.ufrag));
		assert(0 == strcmp("23oU5vsiyBKLHbND/Ql8f7gZ", sdp.pwd));
		assert(true == sdp.ice_options.trickle);
		{
			uint8_t expected_fingerprint[] = { 
				0xD0, 0x44, 0xDF, 0x68, 0x71, 0x39, 0x56, 0x0B,
				0xD3, 0x61, 0x7A, 0xF2, 0x42, 0x5B, 0x1B, 0x0A,
				0xCD, 0xB2, 0x72, 0x84, 0x3A, 0xDE, 0x0F, 0x22,
				0xCA, 0x8C, 0xB0, 0x06, 0x0A, 0x8D, 0xA2, 0x00
			};
			assert(0 == memcmp(sdp.fingerprint.sha256, expected_fingerprint, 32));
		}
		assert(SDP_MODE_RECEIVE_ONLY == sdp.mode);
		assert(true == sdp.rtcp_mux);
		assert(true == sdp.rtcp_rsize);
	}

	// Test parser against Safari offer
	{
		sdp_t sdp;
		assert(0 == sdp_parse(&sdp, safari));
		assert(0 == sdp.version);
		assert(0 == strcmp("-", sdp.username));
		assert(0 == strcmp("3389190485417077944", sdp.session_id));
		assert(0 == strcmp("2", sdp.session_version));
		assert(0 == sdp.start_time);
		assert(0 == sdp.stop_time);
		{
			assert(9 == sdp.video.count);
			assert(9 == sdp.video.port);
			unsigned int expected_type[] = {
				96, 97, 98, 99, 100, 101, 127, 125, 104
			};
			for (int i = 0; i < sdp.video.count; i++) {
				assert(expected_type[i] == sdp.video.params[i].type);
			}
		}
		assert(0 == strcmp("yMtQ", sdp.ufrag));
		assert(0 == strcmp("92GWQlqPVFfVjlxV2qSlQxEq", sdp.pwd));
		assert(true == sdp.ice_options.trickle);
		{
			uint8_t expected_fingerprint[] = { 
				0xD7, 0x41, 0xA3, 0x34, 0xFC, 0x54, 0x27, 0xFD,
				0xD1, 0x2A, 0x58, 0x1D, 0x9E, 0x01, 0x8A, 0xC8,
				0xA9, 0xF3, 0xE0, 0xBE, 0x66, 0xB3, 0xD9, 0x58,
				0xFC, 0x7D, 0x59, 0xA7, 0xBA, 0xD6, 0x99, 0xF3
			};
			assert(0 == memcmp(sdp.fingerprint.sha256, expected_fingerprint, 32));
		}
		assert(SDP_MODE_SEND_AND_RECEIVE == sdp.mode);
		assert(true == sdp.rtcp_mux);
		assert(true == sdp.rtcp_rsize);
	}

	// Test SDP serialize
	{
		char str[2048];
		sdp_t sdp = {
			.ice_options.trickle = true,
			.rtcp_mux = true
		};
		assert(0 == sdp_serialize(str, sizeof(str), &sdp));
		fprintf(stderr, "%s", str);
	}

	return 0;
}
