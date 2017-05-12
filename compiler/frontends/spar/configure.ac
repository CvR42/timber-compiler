dnl Process this file with autoconf to produce a configure script.
AC_INIT(analyze.c)

if test x$KAFFEROOT = x; then
  AC_MSG_ERROR("You must set \$KAFFEROOT to the path of Kaffe")
fi

KAFFEPROBE=$KAFFEROOT/libraries/javalib/kaffe/lang/DummyClassLoader.java;

if test ! -f $KAFFEPROBE; then
  AC_MSG_ERROR("Environment variable KAFFEROOT has incorrect value \'$KAFFEROOT\'. File $KAFFEPROBE should be there")
fi

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

dnl Checks for programs.
AC_PROG_YACC
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_INSTALL

AC_CHECK_PROG(BUILDDEPENDS,g++,g++ -MM)

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(limits.h malloc.h unistd.h)

dnl Checks for library functions.
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(strerror)

dnl Check for system services
dnl AC_EXEEXT
dnl AC_OBJEXT

AC_MSG_CHECKING(for current directory)
SPAR_CWD=`pwd`
AC_MSG_RESULT($SPAR_CWD)

AC_SUBST(SPAR_CWD)

AC_MSG_CHECKING(for prefix)
if test x$prefix = xNONE; then
        spar_cv_prefix=$ac_default_prefix
else
        spar_cv_prefix=$prefix
fi
AC_MSG_RESULT($spar_cv_prefix)

AC_DEFINE_UNQUOTED(INSTALL_SPARLIB_PATH,"$spar_cv_prefix/lib/sparlib")
AC_DEFINE_UNQUOTED(DEVEL_SPARLIB_PATH,"$SPAR_CWD/../sparlib")

dnl AC_OUTPUT(doc/Makefile Makefile testsuite/subpack/Makefile testsuite/Makefile)
AC_CONFIG_HEADER(config.h)
AC_OUTPUT(Makefile)
