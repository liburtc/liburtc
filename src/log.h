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

#ifndef _URTC_LOG_H
#define _URTC_LOG_H

#include <stdio.h>

#ifdef _cplusplus
extern "C" {
#endif

enum level {
	TRACE = 0,
	DEBUG,
	INFO,
	WARN,
	ERROR,
	FATAL,
	NUM_LEVELS // must be last
};

static const char *logl[NUM_LEVELS] = {
	"\033[0;37m",	// trace	(gray)
	"\033[0;32m",	// debug	(green)
	          "",	// info		(white)
	"\033[0;33m",	// warn		(yellow)
	"\033[0;35m",	// error	(magenta)
	"\033[0;31m"	// fatal	(red)
};

static const char *logn[NUM_LEVELS] = {
	"[trace] ",		// trace	(gray)
	"[debug] ",		// debug	(green)
	"[info] ",		// info		(white)
	"[warn] ",		// warn		(yellow)
	"[error] ",		// error	(magenta)
	"[fatal] "		// fatal	(red)
};

static const char *logr[NUM_LEVELS] = {
	"\033[0m\n",			// trace
	"\033[0m\n",			// debug
	       "\n",			// info
	"\033[0m\n",			// warn
	"\033[0m\n",			// error
	"\033[0m\n"				// fatal
};

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

#define log(lvl, format, ...) \
	do { \
		fprintf(stderr, "%s" AT " %s" format "%s", logl[lvl], logn[lvl], ##__VA_ARGS__, logr[lvl]); \
	} while (0);

#ifdef _cplusplus
}
#endif

#endif // _URTC_LOG_H
