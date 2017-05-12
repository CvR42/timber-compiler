dnl Process this file with autoconf to produce a configure script.
AC_INIT(bclus.cc)

dnl If there is a --with-backend, use that as compiler.
dnl We do that by simply setting the environment variable CXX. 
dnl The AC_PROG_CXX test will pick that up later.
AC_ARG_WITH(backend, [AC_HELP_STRING([--with-backend],[use ARG as backend of the Timber compiler])],
export CCC=$withval)

AC_CONFIG_SUBDIRS(glue io)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_RANLIB

if test "$GXX" = "yes"; then
  CXXFLAGS="$CXXFLAGS -Wall -W -ansi -pedantic -Woverloaded-virtual -Wcast-qual -Wundef -Wpointer-arith -Wcast-align -Wwrite-strings -Wconversion -Wmissing-prototypes -O3"
fi

dnl For now, the dependencies are built with g++, no matter what the
dnl compiler is that is used for other stuff.
AC_CHECK_PROG(BUILDDEPENDS,g++,g++ -MM)

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(limits.h)

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
AC_TYPE_SIGNAL

AC_OUTPUT(Makefile)
