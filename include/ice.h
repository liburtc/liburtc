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

#ifndef URTC_ICE_H
#define URTC_ICE_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

#include <sys/select.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum number of ICE bases. Each network interface address is a base. */
#define	ICE_MAX_BASES	16

/* Maximum number of supported ICE (i.e. STUN or TURN) servers */
#define	ICE_MAX_SERVERS	16


/////////////////////////////  TYPE DEFINITIONS  /////////////////////////////

// STUN or TURN server
typedef struct {
	char *url;
	char *username;						/* ignored for stun urls */
	char *credential;					/* ignored for stun urls */
} urtc_ice_server_t;

// ICE base
typedef struct {
} ice_base_t;

// ICE component
typedef enum {
	ICE_COMPONENT_NULL = 0,
	ICE_COMPONENT_RTP,
	ICE_COMPONENT_RTCP
} ice_component_t;

// ICE candidate
typedef struct {
	ice_component_t component;
	uint16_t        port;
	uint32_t        priority;
} ice_candidate_t;

typedef struct {
	ice_candidate_t local;
	ice_candidate_t remote;
} ice_candidate_pair_t;

// See https://tools.ietf.org/html/rfc8445#section-6.1.2.1
typedef enum {
	CHECKLIST_STATE_RUNNING = 0,
	CHECKLIST_STATE_COMPLETED,
	CHECKLIST_STATE_FAILED
} ice_checklist_state_t;

typedef struct {
	ice_checklist_state_t state;
} ice_checklist_t;

// ICE agent object
typedef struct {
	pthread_t	    thread;
	ice_checklist_t checklist;
	ice_base_t      bases[ICE_MAX_BASES];

	int nfds;
	fd_set readfds;
	fd_set writefds;
	fd_set errorfds;

	struct timeval	ta;

	bool cancelled;
} ice_agent_t;


///////////////////////////  FORWARD DECLARATIONS  ///////////////////////////

/**
 * Create new ICE agent
 *
 * Each agent creates a separate thread for processing network i/o events.
 *
 * \param agent[out] Pointer to agent object pointer
 *
 * \return 0 on success. Negative on error.
 */
int ice_agent_create(ice_agent_t **agent);

/**
 * Destroy ICE agent
 *
 * Stops agent thread (if running) and frees memory.
 *
 * \param agent[in] Pointer to agent object
 *
 * \return 0 on success. Negative on error.
 */
int ice_agent_destroy(ice_agent_t *agent);

int ice_gather_host_candidates();

#ifdef __cplusplus
}
#endif

#endif /* URTC_ICE_H */
