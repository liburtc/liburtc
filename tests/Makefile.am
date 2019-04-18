# Build test programs (run with 'make check')
TESTS = $(check_PROGRAMS)
check_PROGRAMS = \
	g711_test \
	mdns_test \
	sdp_test \
	uuid_test

g711_test_CFLAGS = -I$(top_srcdir)/src
g711_test_SOURCES = \
	g711_test.c \
	$(top_srcdir)/src/g711.c \
	$(top_srcdir)/src/g711_tables.c
g711_test_LDADD = $(top_builddir)/src/liburtc.la

mdns_test_CFLAGS = -I$(top_srcdir)/src
mdns_test_SOURCES = \
	mdns_test.c \
	$(top_srcdir)/src/mdns.c
mdns_test_LDADD = $(top_builddir)/src/liburtc.la

sdp_test_CFLAGS = -I$(top_srcdir)/src
sdp_test_SOURCES = \
	sdp_test.c \
	$(top_srcdir)/src/sdp.c
sdp_test_LDADD = $(top_builddir)/src/liburtc.la

uuid_test_CFLAGS = -I$(top_srcdir)/src
uuid_test_SOURCES = \
	uuid_test.c \
	$(top_srcdir)/src/uuid.c
uuid_test_LDADD = $(top_builddir)/src/liburtc.la
