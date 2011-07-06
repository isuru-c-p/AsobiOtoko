AC_INIT([AsobiOtoko], [0.1], [isuru.net])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC


AC_CONFIG_HEADERS([config.h])

AC_CHECK_HEADERS([SDL.h],[],
		[AC_CHECK_HEADERS([SDL/SDL.h],[],
		[AC_MSG_ERROR([need sdl.h to build])]
		)]
	)

AC_CHECK_LIB(SDL,SDL_Init,[],
	[AC_MSG_ERROR([need sdl library to build])])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UINT16_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([bzero])
	
AC_CONFIG_FILES([
Makefile
src/Makefile
])
AC_OUTPUT
