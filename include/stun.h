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

#ifndef URTC_STUN_H
#define URTC_STUN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SIZEOF_STUN_HDR				20

// STUN Classes
#define STUN_CLASS_REQUEST			0
#define STUN_CLASS_INDICTATION		1
#define STUN_CLASS_SUCCESS			2
#define STUN_CLASS_ERROR			3

// STUN Methods
#define STUN_METHOD_RESERVED		0
#define STUN_METHOD_BINDING			1


/////////////////////////////  TYPE DEFINITIONS  /////////////////////////////

typedef struct {
	int rc;								// Retransmission count
	int rto;							// Retransmission timeout (in ms)
	int sockfd;							// Socket file descriptor
} stun_client_t;

typedef struct {
	char *server;
} stun_client_config_t;

// STUN Message Header
typedef struct __attribute__((__packed__)) {
	uint16_t reserved:2;				// Upper two bits are 0
	uint16_t type:14;					// 14-bit type
	uint16_t msglen;					// Message length
	uint32_t cookie;					// Magic cookie
	uint8_t  txid[12];					// Transaction ID
} stun_msg_hdr_t;

// STUN Attribute
typedef struct {
	uint16_t type;						// Attribute type
	uint16_t len;						// Value length, prior to padding
	uint8_t *val;						// Pointer to attribute value
} stun_attr_t;

// STUN Attribute: MAPPED-ADDRESS
typedef struct __attribute__((__packed__)) {
	uint8_t reserved;
	uint8_t family;
	uint16_t port;
	union {
		uint8_t ipv4[4];				// IPv4 address is 32-bits
		uint8_t ipv6[16];				// IPv6 address is 128-bits
	};
} stun_attr_mapped_address_t;

// STUN Attribute: XOR-MAPPED-ADDRESS
typedef stun_attr_mapped_address_t stun_attr_xor_mapped_address_t;

// STUN Attribute: USERNAME


//////////////////////////////////  MACROS  //////////////////////////////////

// Get class from message header type
#define CLASS(typ) (((typ >> 7) & 0x2) | ((typ >> 4) & 0x1))

// Get method from message header type
#define METHOD(typ) (((typ >> 2) & 0xF80) | ((typ >> 1) & 0x70) | (typ & 0xF))

#define TYPE(cls, mthd) ( \
		((mthd & 0xF80) << 2) | \
		((cls & 0x2) << 7) | \
		((mthd & 0x70) << 1) | \
		((cls & 0x1) << 4) | \
		(mthd & 0xF) \
)


///////////////////////////  FORWARD DECLARATIONS  ///////////////////////////

/**
 * (callback) Binding request response handler
 *
 * @param[in] name
 * @param[in] arg  User argument
 */
typedef void (*stun_client_binding_response_callback)(
	const char *name,
	      void *arg
);

/**
 * Sends a STUN binding request to the specified server
 *
 * @param[in] srv  Server (hostname or IP address)
 * @param[in] cb   Handler to call when a response is received
 * @param[in] arg  Optional user argument to pass to callback
 */
err_t stun_client_binding_request(
	runloop_t *rl,
	const char *srv,
	stun_client_binding_response_callback cb,
	void *arg
);

#ifdef __cplusplus
}
#endif

#endif /* URTC_STUN_H */
