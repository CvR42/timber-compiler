dnl Process this file with autoconf to produce a configure script.
AC_INIT(spar-runner.sh)

dnl If there is a --with-backend, use that as compiler.
dnl We do that by simply setting the environment variable CXX. 
dnl The AC_PROG_CXX test will pick that up later.
AC_ARG_WITH(backend, [AC_HELP_STRING([--with-backend],[use ARG as backend of the Timber compiler])],
export CCC=$withval)

if test x$KAFFEROOT = x; then
  AC_MSG_ERROR("You must set \$KAFFEROOT to the path of Kaffe")
fi

KAFFEPROBE=$KAFFEROOT/libraries/javalib/kaffe/lang/DummyClassLoader.java;

if test ! -f $KAFFEPROBE; then
  AC_MSG_ERROR("Environment variable KAFFEROOT has incorrect value \'$KAFFEROOT\'. File $KAFFEPROBE should be there")
fi

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_INSTALL

dnl Checks for libraries.

dnl Checks for header files needed by generated programs
AC_HEADER_STDC
AC_CHECK_HEADERS(limits.h malloc.h unistd.h)

AC_MSG_CHECKING(for compiler root directory)
SPAR_CWD=`pwd`
SPAR_ROOT=`echo $SPAR_CWD | sed -e s@/driver@@`
AC_MSG_RESULT($SPAR_ROOT)

AC_SUBST(SPAR_ROOT)

AC_MSG_CHECKING(for prefix)
if test x$prefix = xNONE; then
        spar_cv_prefix=$ac_default_prefix
else
        spar_cv_prefix=$prefix
fi
AC_MSG_RESULT($spar_cv_prefix)

AC_DEFINE_UNQUOTED(INSTALL_SPARLIB_PATH,"$spar_cv_prefix/lib/sparlib")
AC_DEFINE_UNQUOTED(DEVEL_SPARLIB_PATH,"$SPAR_ROOT/frontends/sparlib")

AC_OUTPUT(runner-develop-config.sh runner-install-config.sh Makefile)
