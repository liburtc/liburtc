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
