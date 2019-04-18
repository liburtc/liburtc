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

#ifndef URTC_CONFIG_H
#define URTC_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ice.h"

/* Peer connection configuration */
typedef struct urtc_config {
	urtc_ice_server_t ice_servers[ICE_MAX_SERVERS];

	// TODO video_source_t *vsrc;
	// TODO video_sink_t   *vsnk;

	// TODO audio_source_t *asrc;
	// TODO audio_sink_t   *asnk;

	// TODO data_source_t  *dsrc;
	// TODO data_sink_t    *dsnk;
} urtc_config_t;

#ifdef __cplusplus
}
#endif

#endif /* URTC_CONFIG_H */
