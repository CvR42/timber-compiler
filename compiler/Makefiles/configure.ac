dnl Process this file with autoconf to produce a configure script.
AC_INIT(Make.vars.in)

AC_ARG_WITH(pthreads,[AC_HELP_STRING([--with-pthreads],[create pthreads glue library])],
have_pthreads=$withval
,
have_pthreads=no
)
AC_ARG_WITH(panda,   [AC_HELP_STRING([--with-panda],[create panda glue library])],
have_panda=$withval
,
have_panda=no
)
AC_ARG_WITH(mpi,     [AC_HELP_STRING([--with-mpi],[create mpi glue library])],
have_mpi=$withval
,
have_mpi=no
)
AC_ARG_WITH(pvm,     [AC_HELP_STRING([--with-pvm],[create pvm glue library])],
have_pvm=$withval
,
have_pvm=no
)
AC_ARG_WITH(trimedia,[AC_HELP_STRING([--with-trimedia],[create trimedia glue library])],
have_trimedia=$withval
,
have_trimedia=no
)
AC_ARG_ENABLE(das,    [AC_HELP_STRING([--enable-das],[use Myrinet on DAS])],
on_das=$enableval
,
on_das=no
)

mpiflavour=$have_mpi
if test "$have_mpi" = lam; then
    have_mpi=yes
fi
if test "$have_mpi" = no; then
    mpiflavour=lam
fi
if test "$have_mpi" = yes; then
    mpiflavour=lam
fi
if test "$have_mpi" = mpich; then
    have_mpi=yes
fi
if test "$have_mpi" = no; then
    AC_MSG_RESULT(No MPI support)
else
    AC_MSG_RESULT(Using MPI implementation '$mpiflavour')
fi

AC_CHECKING(consistency)
if test "$mpiflavour" != lam && test "$mpiflavour" != mpich ; then
	AC_MSG_RESULT(Invalid MPI version: '$mpiflavour')
	AC_MSG_ERROR(Giving up)
fi

if test "$on_das" = yes && test "$mpiflavour" = lam ; then
	AC_MSG_RESULT(lam not available on DAS)
	AC_MSG_ERROR(Giving up)
fi

if test "$have_pvm" = yes ; then
	AC_CHECKING(pvm files)
	if test "x$PVM_ROOT" = x ; then
		pvm_includedir=/usr/include
		pvm_libdir=/usr/lib
	else
		pvm_includedir=$PVM_ROOT/include
		pvm_libdir=$PVM_ROOT/lib/LINUX
	fi
	if test ! -f $pvm_includedir/pvm3.h; then
		AC_MSG_RESULT(Pvm not installed or PVM_ROOT incorrectly set)
		AC_MSG_ERROR(Giving up)
	fi
	if test ! -f $pvm_libdir/libpvm3.a; then
		AC_MSG_RESULT(Pvm not installed or PVM_ROOT incorrectly set)
		AC_MSG_ERROR(Giving up)
	fi
	AC_SUBST(pvm_includedir)
	AC_SUBST(pvm_libdir)
fi

if test "$have_mpi" = yes && test "$mpiflavour" = lam ; then
	AC_CHECKING(lam:mpi files)
	if test -d /usr/local/lam ; then
		lam_includedir=/usr/local/lam/include
		lam_libdir=/usr/local/lam/lib
	else
		lam_includedir=/usr/lib/lam/include
		lam_libdir=/usr/lib/lam/lib
	fi
	if test -f $lam_libdir/libtrillium.a; then
		lam_libs="-lmpi -ltstdio -ltrillium -largs -lt"
	else
		lam_libs="-lmpi"
	fi
	if test ! -f $lam_includedir/mpi.h; then
		AC_MSG_RESULT(Lam:mpi not installed)
		AC_MSG_ERROR(Giving up)
	fi
	if test ! -f $lam_libdir/libmpi.a; then
		AC_MSG_RESULT(Lam:mpi not installed)
		AC_MSG_ERROR(Giving up)
	fi
	AC_SUBST(lam_includedir)
	AC_SUBST(lam_libdir)
	AC_SUBST(lam_libs)
fi

if test "$have_mpi" = yes && test "$mpiflavour" = mpich ; then
	AC_CHECKING(mpich:mpi files)
	mpich_cxx_flags=`./get_mpich_cxxflags`
	mpich_libdir_flags=`./get_mpich_libdirflags`
	mpich_libs_flags=`./get_mpich_libsflags`
	AC_SUBST(mpich_cxx_flags)
	AC_SUBST(mpich_libdir_flags)
	AC_SUBST(mpich_libs_flags)
fi

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB
AC_CHECK_PROG(BUILDDEPENDS,g++,g++ -MM)

dnl Checks for libraries.

dnl Checks for header files.

dnl The substitutions
AC_SUBST(have_pthreads)
AC_SUBST(have_panda)
AC_SUBST(have_mpi)
AC_SUBST(have_pvm)
AC_SUBST(have_trimedia)
AC_SUBST(on_das)
AC_SUBST(mpiflavour)

AC_OUTPUT(Make.vars spar-preamble)
