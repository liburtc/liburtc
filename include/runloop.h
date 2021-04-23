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

/**
 * @file runloop.h
 * Event loop
 * A more elaborate file description.
 */

#ifndef URTC_RUNLOOP_H
#define URTC_RUNLOOP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <poll.h>
#include <pthread.h>
#include <stdbool.h>

typedef void *(*callback_t)(int fd, void *arg);

typedef struct {
	// File descriptor sets for poll().
	struct pollfd *fds;
	nfds_t nfds;

	// Callback array. Index into array is file descriptor.
	callback_t *callbacks;
	void **args;

	// Pipe for aborting blocked poll(). Allows dynamic adds/removes.
	int abortpipe[2];
	bool done;

	// Run loop thread ID
	pthread_t tid;
} runloop_t;

/**
 * Create a new run loop
 *
 * Each run loop creates a separate thread which then repeatedly poll()s
 * for events. Upon receiving an event, the run loop calls the respective
 * file descriptor's callback function.
 *
 * @param[out] rl  Pointer to run loop struct
 */
int urtc__runloop_create(runloop_t *rl);

/**
 * Add file descriptor to run loop
 *
 * @param rl      Run loop
 * @param fd      File descriptor
 * @param events  Events to monitor (see struct pollfd)
 * @param cb      Callback to run on activity
 * @param arg     User-specified argument to pass to callback
 *
 * @return Zero on success. Negative on error.
 */
int urtc__runloop_add(
	runloop_t *rl,
	int fd,
	short events,
	void *(*cb)(int fd, void *arg),
	void *arg
);

/**
 * Remove a file descriptor from the run loop. No more events will be
 * processed for this file descriptor.
 *
 * @param rl Run loop.
 * @param fd File descriptor.
 *
 * @return Zero on success. Negative on error.
 */
int urtc__runloop_remove(runloop_t *rl, int fd);

int urtc__runloop_join(runloop_t *rl);

/**
 * Destroy a run loop
 *
 * @param rl Pointer to run loop object
 *
 * @return Zero on success; negative on error. On error, errno is set.
 */
int urtc__runloop_destroy(runloop_t *rl);

#ifdef __cplusplus
}
#endif

#endif /* URTC_RUNLOOP_H */
