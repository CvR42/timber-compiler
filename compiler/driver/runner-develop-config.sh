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
FRONT=/Users/reeuwijk/timber/compiler/frontends/spar/sparfront
VNUS=/Users/reeuwijk/timber/compiler/backends/vnus/vnus
CXX=g++

SPARRUNTIMELIB=/Users/reeuwijk/timber/compiler/frontends/spar-rtl
SPARRUNTIMEINCLUDE=/Users/reeuwijk/timber/compiler/frontends/spar-rtl
VNUSRUNTIMELIB=/Users/reeuwijk/timber/compiler/backends/runtime
VNUSRUNTIMEINCLUDE=/Users/reeuwijk/timber/compiler/backends/runtime
INITIAL=/Users/reeuwijk/timber/compiler/engines/mapper/initial
SPMD=/Users/reeuwijk/timber/compiler/engines/mapper/spmd
ALIGN=/Users/reeuwijk/timber/compiler/engines/align/align
SPARLIBPATH=/Users/reeuwijk/timber/compiler/frontends/sparlib
