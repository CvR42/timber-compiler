dnl Process this file with autoconf to produce a configure script.
AC_INIT(service.cc)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_INSTALL

AC_CHECK_PROG(BUILDDEPENDS,g++,g++ -MM)

dnl Check for the path to Tm.
AC_PATH_PROG( TMROOT, tm, no )
if test $TMROOT = no ; then
	AC_PATH_PROG( TMROOT, tm.exe, no )
	if test $TMROOT = no ; then
		AC_MSG_RESULT(not found)
		echo "Cannot find the program 'tm' or 'tm.exe'. See README for details.'" 
		AC_MSG_ERROR(Giving up)
	else
		TMROOT=`echo $TMROOT | sed 's![\\/]bin[\\/]tm.exe!!'`
	fi
else
        TMROOT=`echo $TMROOT | sed 's![\\/]bin[\\/]tm!!'`
fi
AC_MSG_CHECKING(for Tm prefix)
if test ! -f $TMROOT/include/tmc.h; then
	echo "$TMROOT/include/tmc.h is not there." 
	AC_MSG_ERROR(Giving up)
fi
AC_MSG_RESULT($TMROOT)
AC_SUBST(TMROOT)

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC

dnl Checks for library functions.
AC_CHECK_FUNCS(strerror)

AC_CONFIG_HEADER(config.h)
AC_OUTPUT(Makefile)
