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

#ifndef URTC_PEERCONN_H
#define URTC_PEERCONN_H

#include "config.h"
#include "ice.h"
#include "sdp.h"
#include "runloop.h"

#ifdef __cplusplus
extern "C" {
#endif


/****************************  TYPE DEFINITIONS  ****************************/

// Event callback function type. Takes and returns void pointers.
typedef void *(*event_callback_t)(void *arg);

// Peer connection
typedef struct urtc_peerconn {
	uint8_t n;

	// Interactive connectivity establishment agent
	ice_agent_t *agent;

	// Local and remote session descriptions
	sdp_t *ldesc;
	sdp_t *rdesc;

	// Event callbacks
	void (*onIceCandidate)(const char *cand, void *arg);
	void (*onIceGatheringStateChange)(int state);
	void (*onIceConnectionStateChange)(int state);
	void (*onSignalingStateChange)(int state);
	void (*onTrack)(int track);

	// Run loop for event handling
	runloop_t runloop;
} urtc_peerconn_t;


/**************************  FORWARD DECLARATIONS  **************************/

/**
 * Create a new peer connection.
 *
 * \param pc[out] New peer connection.
 * \param cfg[in] Peer connection configuration.
 *
 * \return 0 on success. Negative on error.
 */
int urtc_peerconn_create(urtc_peerconn_t **pc, const urtc_config_t *cfg);

/**
 * Adds received ICE candidate to ICE agent.
 *
 * If `cand` is a NULL pointer or empty string, it signals that all remote
 * candidates have been delivered (i.e. end-of-candidates).
 *
 * \param pc[in] Peer connection.
 * \param cand[in] ICE candidate.
 *
 * \return 0 on success. Negative on error.
 */
int urtc_peerconn_add_ice_candidate(urtc_peerconn_t *pc, const char *cand);

/**
 * Set remote description.
 *
 * \param pc[in] Peer connection.
 * \param desc[in] Session description protocol string.
 *
 * \return 0 on success. Negative on error.
 */
int urtc_peerconn_set_remote_description(urtc_peerconn_t *pc, const char *desc);

/**
 * Set local description.
 *
 * \param pc[in] Peer connection.
 * \param desc[in] Session description protocol string.
 *
 * \return 0 on success. Negative on error.
 */
int urtc_peerconn_set_local_description(urtc_peerconn_t *pc, const char *desc);

/**
 * Create offer.
 *
 * \param pc[in] Peer connection.
 * \param offer[out] Session description to send to remote peer. Caller must
 *     free.
 *
 * \return 0 on success. Negative on error.
 */
int urtc_peerconn_create_offer(urtc_peerconn_t *pc, char **offer);

/**
 * Create answer.
 *
 * \param pc[in] Peer connection
 * \param answer[out] Session description to send to remote peer. Caller must
 *     free.
 *
 * \return 0 on success. Negative on error.
 */
int urtc_peerconn_create_answer(urtc_peerconn_t *pc, char **answer);

/**
 * Teardown a peer connection.
 *
 * \param pc[in] Peer connection.
 *
 * \return 0 on success. Negative on error.
 */
int urtc_peerconn_destroy(urtc_peerconn_t *pc);

#ifdef __cplusplus
}
#endif

#endif /* URTC_PEERCONN_H */
