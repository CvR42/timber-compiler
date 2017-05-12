dnl Process this file with autoconf to produce a configure script.
AC_INIT(align.cc)

dnl Checks for programs.
AC_PROG_YACC
AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_INSTALL

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
AC_CHECK_HEADERS(malloc.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_FUNC_ALLOCA
AC_CHECK_FUNCS(uname)

AC_OUTPUT(Makefile)
