# timber-compiler

The Timber compiler implements Spar/Java, a superset of Java with
additional language constructs for high-performance computation. The
additions include support for data parallelism and task parallelism,
tuples, multi-dimensional arrays, generic classes and interfaces through
templates, complex numbers, and subscript operator overloading.

Timber is implemented as a static whole-program compiler, using a standard
C++ compiler to generate machine code. For now only Gnu C compilers are
supported, but that is mainly a restriction in our configuration system.
We plan to support other compilers in the future.

The Spar language extensions are only supported for source files with
the extension .spar.  For source files with the extension .java or .jav,
the Timber compiler behaves like a standard Java compiler. The main
restrictions are (a) no support for dynamic class loading (because we
do static whole-program compilation), and (b) no support for threads
(because we provide our own parallelization constructs).

Since the compiler uses well-defined and accessible internal data
structures, we believe that it is also very suitable as an experimental
compiler.

Timber is released under the Gnu Public License (GPL).

The Timber compiler has been designed to be portable, but it has mainly
been tested on recent Debian i386 Linux systems.  It requires two external
packages: a recent version of Tm, the template manager; and Kaffe version
1.0.6. Both can be downloaded from the page mentioned above.
