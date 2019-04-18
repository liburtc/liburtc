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
	urtc_sdp_t *ldesc;
	urtc_sdp_t *rdesc;

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
