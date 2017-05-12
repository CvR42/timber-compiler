dnl Process this file with autoconf to produce a configure script.
AC_INIT(Makefile.in)

dnl if test x$prefix = xNONE; then
dnl         timber_cv_prefix=$ac_default_prefix
dnl else
dnl         timber_cv_prefix=$prefix
dnl fi
dnl AC_MSG_RESULT($timber_cv_prefix)

dnl AC_MSG_CHECKING(prefix exists)
dnl if test ! -d $timber_cv_prefix ; then
dnl 	AC_MSG_RESULT(it does not)
dnl 	AC_MSG_ERROR(Directory $timber_cv_prefix does not exist)
dnl fi
dnl AC_MSG_RESULT(it does)


dnl Make the specific configure flag help texts for this distribution show up
dnl in the top-level configure file.
AC_ARG_WITH(mpi,     [AC_HELP_STRING([--with-mpi],[enable parallization using the MPI library])])
AC_ARG_WITH(pvm,     [AC_HELP_STRING([--with-pvm],[enable parallelization using the PVM library])])
AC_ARG_WITH(backend, [AC_HELP_STRING([--with-backend],[use ARG as backend of the Timber compiler])])
AC_ARG_ENABLE(das,    [AC_HELP_STRING([--enable-das],[use Myrinet on DAS])])
dnl We don't need Tm in this directory. We check here anyway because we want
dnl to catch configuration errors early.
AC_PATH_PROG( TMPATH, tm, no )
if test $TMPATH = no ; then
	AC_PATH_PROG( TMPATH, tm.exe, no )
	if test $TMPATH = no ; then
		AC_MSG_RESULT(not found)
		echo "Cannot find the program 'tm' or 'tm.exe'. See README for details.'" 
		AC_MSG_ERROR(Giving up)
	else
		TMPATH=`echo $TMPATH | sed 's![\\/]bin[\\/]tm.exe!!'`
	fi
else
        TMPATH=`echo $TMPATH | sed 's![\\/]bin[\\/]tm!!'`
fi
AC_MSG_RESULT($TMPATH)
if test ! -f $TMPATH/include/tmc.h; then
	echo "$TMPATH/include/tmc.h is not there." 
	AC_MSG_ERROR(Giving up)
fi

AC_CONFIG_SUBDIRS(Makefiles driver frontends backends engines)

dnl Checks for programs.
AC_PROG_MAKE_SET

dnl Checks for libraries.

dnl Checks for header files.

AC_OUTPUT(Makefile)
