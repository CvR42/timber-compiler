#! /bin/sh
#
# Driver script for the spar compiler, to make it look like an
# ordinary compiler such as gcc.
#
# See the function 'showusage' for the usage of this command.
VERSION="2.0"
# Paths
TMPDIR=${TMP:-/tmp}
#
# Places of executables
prefix=/Volumes/User/reeuwijk/timber/compiler
exec_prefix=${prefix}
FRONT=${exec_prefix}/libexec/sparfront
VNUS=${exec_prefix}/bin/vnus
CXX=c++

SPARRUNTIMELIB=${exec_prefix}/lib
SPARRUNTIMEINCLUDE=${prefix}/include/timber
VNUSRUNTIMELIB=${exec_prefix}/lib
VNUSRUNTIMEINCLUDE=${prefix}/include/timber
INITIAL=${exec_prefix}/libexec/spar-initial
ALIGN=${exec_prefix}/libexec/spar-align
SPMD=${exec_prefix}/libexec/spar-spmd
SPARLIBPATH=${exec_prefix}/lib/sparlib

