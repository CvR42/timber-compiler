#! /bin/sh
#
# Driver script for the spar compiler, to make it look like an
# ordinary compiler such as gcc.
#
# See the function 'showusage' for the usage of this command.
VERSION=": development"
# Paths
TMPDIR=${TMP:-/tmp}
#
# Places of executables
FRONT=/Volumes/User/reeuwijk/timber/compiler/frontends/spar/sparfront
VNUS=/Volumes/User/reeuwijk/timber/compiler/frontends/spar/../../backends/vnus/vnus
CXX=c++

SPARRUNTIMELIB=/Volumes/User/reeuwijk/timber/compiler/frontends/spar/../spar-rtl
SPARRUNTIMEINCLUDE=/Volumes/User/reeuwijk/timber/compiler/frontends/spar/../spar-rtl
VNUSRUNTIMELIB=/Volumes/User/reeuwijk/timber/compiler/frontends/spar/../../backends/runtime
VNUSRUNTIMEINCLUDE=/Volumes/User/reeuwijk/timber/compiler/frontends/spar/../../backends/runtime
INITIAL=/Volumes/User/reeuwijk/timber/compiler/frontends/spar/../../engines/mapper/initial
SPMD=/Volumes/User/reeuwijk/timber/compiler/frontends/spar/../../engines/mapper/spmd
ALIGN=/Volumes/User/reeuwijk/timber/compiler/frontends/spar/../../engines/align/align
SPARLIBPATH=/Volumes/User/reeuwijk/timber/compiler/frontends/spar/../sparlib
