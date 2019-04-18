AC_INIT([liburtc], [1.0.0], [chris@hiszpanski.name])

# custom m4 macros within m4/ subdirectory
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wall -Werror foreign no-dependencies subdir-objects])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_LIBOBJ_DIR([src])

# for pthread support on linux
AX_PTHREAD(,[AC_MSG_ERROR([Could not configure pthreads support])])

AM_PROG_AR
LT_INIT

AC_CONFIG_FILES([
	Makefile
	examples/Makefile
	src/Makefile
	tests/Makefile
])
AC_OUTPUT
