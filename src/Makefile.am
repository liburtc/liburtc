lib_LTLIBRARIES = liburtc.la
liburtc_la_SOURCES = b64.c g711.c g711_tables.c mdns.c prng.c sdp.c urtc.c \
						uuid.c
include_HEADERS = urtc.h

# for pthreads support on linux
liburtc_la_CFLAGS  = $(PTHREAD_CFLAGS)
liburtc_la_LDFLAGS = $(PTHREAD_LDFLAGS)
liburtc_la_LIBADD  = $(PTHREAD_LIBS)
