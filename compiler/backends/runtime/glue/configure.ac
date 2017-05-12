dnl Process this file with autoconf to produce a configure script.
AC_INIT(mpi.cc)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_INSTALL

AC_CHECK_PROG(BUILDDEPENDS,g++,g++ -MM)

if test "$GXX" = "yes"; then
  CXXFLAGS="$CXXFLAGS -Wall -W -ansi -pedantic -Woverloaded-virtual -Wcast-qual -Wundef -Wpointer-arith -Wcast-align -Wwrite-strings -Wconversion -Wmissing-prototypes -Wmissing-declarations"
fi

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(limits.h)

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
AC_TYPE_SIGNAL

AC_OUTPUT(Makefile)
