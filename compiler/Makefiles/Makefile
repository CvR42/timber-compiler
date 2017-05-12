MAKEFILE = Makefile

MPICHINSPECTORS = get_mpich_cxxflags get_mpich_libdirflags get_mpich_libsflags

DISTFILES = $(MAKEFILE) Make.dist Make.libs Make.rules Make.tests \
    Make.vars.in Make.std Make.vars.spar Make.configure configure \
    configure.ac spar-preamble.in $(MPICHINSPECTORS)


include Make.dist
include Make.std

# These are different from the regular ones, hence we write them out
# explicitly.

all: Make.vars

configure: configure.ac
	autoconf

config.status: configure
	./config.status --recheck

Make.vars: Make.vars.in config.status
	./config.status

JUNK += config.log config.cache
PRECIOUSJUNK += config.status configure
