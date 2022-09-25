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

#ifndef _URTC_LOG_H
#define _URTC_LOG_H

#include <stdio.h>

#ifdef _cplusplus
extern "C" {
#endif

enum urtc_level {
    URTC_TRACE = 0,
    URTC_DEBUG,
    URTC_INFO,
    URTC_WARN,
    URTC_ERROR,
    URTC_FATAL,
    NUM_LEVELS // must be last
};

static const char *logl[NUM_LEVELS] = {
    "\033[0;37m",   // trace    (gray)
    "\033[0;32m",   // debug    (green)
              "",   // info     (white)
    "\033[0;33m",   // warn     (yellow)
    "\033[0;35m",   // error    (magenta)
    "\033[0;31m"    // fatal    (red)
};

static const char *logn[NUM_LEVELS] = {
    "[trace] ",     // trace    (gray)
    "[debug] ",     // debug    (green)
    "[info] ",      // info     (white)
    "[warn] ",      // warn     (yellow)
    "[error] ",     // error    (magenta)
    "[fatal] "      // fatal    (red)
};

static const char *logr[NUM_LEVELS] = {
    "\033[0m\n",            // trace
    "\033[0m\n",            // debug
           "\n",            // info
    "\033[0m\n",            // warn
    "\033[0m\n",            // error
    "\033[0m\n"             // fatal
};

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

#define urtc_log(lvl, format, ...) \
    do { \
        fprintf(stderr, "%s" AT " %s" format "%s", logl[lvl], logn[lvl], ##__VA_ARGS__, logr[lvl]); \
    } while (0);

#ifdef _cplusplus
}
#endif

#endif // _URTC_LOG_H
