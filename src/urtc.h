/**
 * \file urtc.h
 *
 * \brief liburtc header file.
 *
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
 * \mainpage notitle
 *
 * The reference WebRTC implementation, libwebrtc, was developed by Google
 * for the Chrome web browser. It's codebase is large (over 10GB of source),
 * uses uncommon build tools (Bazel), and produces large executables not
 * well suited for constrained embedded devices.
 *
 * liburtc is designed with embedded devices in mind. Only functionality
 * commonly found on embedded devices is implemented. For example, only
 * the H.264 video codec is supported (there is no VP8/VP9 support) as
 * most embedded devices with hardware-accelerated video encoders only
 * support H.264.
 *
 * \section features_sec Features
 * * Small footprint (< 100KB binary)
 * * H.264 support only (no VP8/9 support)
 * * ANSI C99
 *
 */
#ifndef _URTC_H
#define _URTC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opque peer connection structure.
 *
 * Instantiate via urtc_peerconn_create(). Each peer connection creates its
 * own single thread for processing events in a runloop.
 */
typedef struct peerconn urtc_peerconn_t;

/**
 * (callback) Called for each new local ICE candidate discovered
 *
 * The local candidate should be relayed to the remote peer. Note that callback
 * executes on peer connection event loop -- do not block or do other expensive
 * work within the callback.
 *
 * Akin to the `onicecandidate` event handler in `RTCPeerConnection`.
 *
 * \param cand Discovered local candidate string.
 * \param arg User specified argument, see urtc_set_on_ice_candidate().
 *
 * \return 0 on success, negative on error.
 */
typedef int (urtc_on_ice_candidate)(const char *cand, void *arg);

/**
 * (callback) Called to request that video encoder immediately generate an IDR
 *
 * H.264 video encoders produce I-frames and P-frames. Only I-frames can be
 * decoded independently. To minimize the time-to-first-frame (TTFF), an
 * I-frame should be sent as soon as a peer connection is established. In the
 * event of picture loss, an I-frame should be sent as soon as possible to
 * re-establish picture.
 *
 * This is an optional callback. If set, it improves the TTFF and picture
 * recovery time from loss of picture.
 */
typedef void (urtc_force_idr)();


/**
 * Create a new peer connection
 *
 * Note that the new peer connection is not yet connected to a peer.
 *
 * Akin to `RTCPeerConnection()` in the WebRTC JS API.
 *
 * \param stun Array of STUN servers to use for discovery of server-reflexive
 *             ICE candidates. Each entry should be a string of the form
 *             `hostname[:port]` (e.g. "stun.l.google.com:19302"). The port
 *             is optional -- if omitted, 3478 is assumed. The final entry in
 *             the array must be NULL. If no STUN servers are provided, only
 *             local area network peers will be discoverable.
 *
 * \return On success, a pointer to new peer connection object is returned.
 *		The peer connection must be destroyed with urtc_peerconn_destroy()
 *		when no longer needed. On error, NULL is returned.
 */
urtc_peerconn_t * urtc_peerconn_create(const char *stun[]);

/**
 * Sets onIceCandidate callback function
 *
 * Callback function will be called when a new local ICE candidate is
 * discovered.
 */
int urtc_set_on_ice_candidate(urtc_peerconn_t *pc, urtc_on_ice_candidate *cb);

/**
 * Adds received remote ICE candidate to peer connection
 *
 * For each ICE candidate received from the remote peer via some external
 * signaling channel, use this function to add the candidate to the peer
 * connection. Candidates are asynchronously used to check for direct
 * peer-to-peer connectivity with the remote peer.
 *
 * Akin to `addIceCandidate` method of `RTCPeerConnection` in the WebRTC JS API.
 *
 * \param pc Peer connection.
 * \param cand An ICE candidate string from remote peer.
 *
 * \return 0 on success, negative on error.
 */
int urtc_add_ice_candidate(urtc_peerconn_t *pc, const char *cand);

/**
 * Creates local description for an answering peer connection
 *
 * When a peer connection is the answering party, generates a local
 * description (in Session Description Protocol). This description
 * should be both sent to remote peer connection via some external
 * signaling channel as well as added to the local peer connection via
 * urtc_set_local_description().
 *
 * Akin to `createAnswer` method of `RTCPeerConnection` in WebRTC JS API.
 *
 * \param pc Peer connection
 * \param answer Pointer to string pointer pointing to generated answer.
 *               Memory internally managed (caller need not and must not free).
 *
 * \return 0 on success, negative on error.
 */
int urtc_create_answer(urtc_peerconn_t *pc, char **answer);

/**
 * Creates a local description for an offering peer connection
 *
 * When a peer connection is the offering party, this function generates a
 * local description (in Session Description Protocol). This description
 * should be both sent to the remote peer connection via some external
 * signaling channel as well as added to the local peer connection via
 * urtc_set_local_description().
 *
 * Akin to `createOffer` method of `RTCPeerConnection` in WebRTC JS API.
 *
 * \param pc Peer connection
 * \param offer Pointer to string pointer pointing to generated offer.
 *              Memory interally managed (caller need no and must not free).
 *
 * \return 0 on success, negative on error.
 */
int urtc_create_offer(urtc_peerconn_t *pc, char **offer);

/**
 * Sets local description for peer connection
 *
 * The local description is a Session Description Protocol string, typically
 * generated by urtc_create_offer() or urtc_create_answer(), depending on
 * whether the local peer connection is the offering or answering party,
 * respectively. Setting the local description begins local ICE candidate
 * discovery.
 *
 * Akin to `setLocalDescription` method of `RTCPeerConnection` in WebRTC JS API.
 *
 * \param pc Peer connection previously created via urtc_peerconn_create().
 * \param desc Local description in Session Description Protocol (SDP)
 *
 * \return 0 on success, negative on error.
 */
int urtc_set_local_description(urtc_peerconn_t *pc, const char *desc);

/**
 * Sets remote description
 *
 * The remote description is a Session Description Protocol string,
 * received from the remote peer via some external signaling channel.
 * Amongst other things, it defines the set of media codecs the remote
 * peer supports.
 *
 * Akin to `setRemoteDescription` method of `RTCPeerConnection` in WebRTC JS
 * API.
 *
 * \param pc Peer connection created by urtc_peerconn_create().
 * \param desc Session description protocol.
 *
 * \return 0 on success, negative on error.
 */
int urtc_set_remote_description(urtc_peerconn_t *pc, const char *desc);

/**
 * Tears down peer connection, closing and freeing all resources
 *
 * \param pc Peer connection.
 */
void urtc_peerconn_destroy(urtc_peerconn_t *pc);

#ifdef __cplusplus
}
#endif

#endif /* _URTC_H */
