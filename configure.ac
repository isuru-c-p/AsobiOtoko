AC_INIT([AsobiOtoko], [0.1], [isuru.net])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CHECK_HEADERS([stdint.h],[],
	[AC_MSG_ERROR([need stdint.h to build])]
	)
AC_CONFIG_FILES([
Makefile
src/Makefile
])
AC_OUTPUT
