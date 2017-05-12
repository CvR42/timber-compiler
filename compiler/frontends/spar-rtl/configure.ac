dnl Process this file with autoconf to produce a configure script.
AC_INIT(Introspection.cc)

dnl If there is a --with-backend, use that as compiler.
dnl We do that by simply setting the environment variable CXX. 
dnl The AC_PROG_CXX test will pick that up later.
AC_ARG_WITH(backend, [AC_HELP_STRING([--with-backend],[use ARG as backend of the Timber compiler])],
export CCC=$withval)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_RANLIB

AC_CHECK_PROG(BUILDDEPENDS,g++,g++ -MM)

if test "$GXX" = "yes"; then
  CXXFLAGS="$CXXFLAGS -Wall -W -ansi -pedantic -Woverloaded-virtual -Wcast-qual -Wundef -Wpointer-arith -Wcast-align -Wwrite-strings -Wconversion"
fi

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h sys/time.h unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_TIME

dnl Checks for library functions.
AC_CHECK_FUNCS(gettimeofday strtod strtol)

AC_OUTPUT(Makefile)
