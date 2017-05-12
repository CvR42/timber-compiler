$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Spar - why do we violate Java standards and conventions?";
&generateHeader( $webpage, $title );
&generate_clickable_img( $webpage, "povray/icon_spar.jpg", "spar/index.html", "spar" );
print <<EOF;
In Spar, we violate a number of standards and conventions of Java.
On this page I will try to express a number of valid complaints against
our work, and I will try to explain why we feel that we had to do it.

<h2>You extend Java; if Spar ever becomes popular, this could fraction the code base into incompatible dialects</h2>
<font color=red>Since the point of Spar is to test a number of language
constructs of our own invention, we don't see how
we can avoid extending the language</font>. We have tried to minimize
the impact on Java, and we do not modify the language for the sake of it.
<p>
The language extensions that we have chosen were designed to be
unintrusive, powerful, and generally useful. We believe that they do
not violate the `spirit' of Java, and that the language extensions are
valuable additions to the language. Who knows, perhaps Sun agrees with us
(or similar proposals from others), and one day extends the language.
<p>
Also, there is a compiler option <code>--java</code> that switches off
all language extensions. You can also selectively switch off language
extensions.

<h2>You do not compile to JVM code, making the compiled code non-portable</h2>
There are two reasons for this. First, <font color=red>this is not easy
to do for a small research group like ours</font>. We have a working
compilation path for native code generation that builds on previous work,
and retargeting everything to JVM is not easy. Moreover, it is not clear
that all the constructs of our intermediate code could easily be expressed
in JVM.
<p>
Another reason not to compile to JVM, is that I am convinced that <font
color=red>using a virtual machine still causes a performance hit</font>.
Yes, I know that there are reports that prove that Just In Time compilers
can cause performance equal to (or even better than) native code, but
there are more than enough reports that suggests that this only applies
to specific program areas.
<p>
The <em>compiler</em> is quite portable and generates fairly portable C++
code (interfacing to the message passing library of the new platform is
probably the most difficult part). In that sense Spar code is portable.

<h2>You do not implement all of Java</h2>
<font color=red>Implementing all of Java, and all of the supporting
classes is a difficult task</font>. Moreover, beyond a certain point Java
implementation issues are less important to us than our global research
goals. It is likely that implementing all the dark and rarely visited
corners of Java will take some time.
<p>
At the moment we can only support classes for which we have source
code, or classes that we have re-implemented from the specification.
We <em>are</em> considering implementing a <code>.class</code>
compiler, which would make it easier for us to support arbitrary
Java classes.

<h2>You do not license the Java compiler from Sun, how can you ever be compatible?</h2>
It is true that we do not have a license from Sun
for the source of their javac compiler. <font color=red>This is a
deliberate choice</font>.  The current Sun licence is quite liberal, but
since <font color=red>we intend to release the Spar compiler under a GPL
license</font>, we cannot be `tainted' with inside knowledge of the Sun
java compiler.  Therefore, the compiler was implemented from the public
language specification in the Java Language Specification.  Besides,
the structure of our compiler is so vastly different from the Sun one,
that having the source code would help us little.
<p>
However, we <font color=red>try as much as possible to follow the
published standard</font>.

<h2>Spar floating point calculations may differ from Java calculations</h2>
Since we map floating point calculations directly on C++ floating point
calculations, this is probably true. <font color=red>Getting this right would require
a lot of effort, would hinder optimization, and would negatively affect
some programs</font>.
<p>
Moreover, as several people have argued (see e.g. the <a
href="http://HTTP.CS.Berkeley.EDU/~darcy/Borneo/">Borneo project</a>), the
current tight specification of Java floating point behavior is detrimental
to floating point calculation. This specification requires the compiler
to throw away floating point precision only to be compatible with the
standard, and encourages the dubious practice of relying of the exact
behavior of a floating point calculation.

<h2>You use templates to provide generic types</h2>
Although using templates for generic types does have some disadvantages, we
consider them less significant than the advantages.
<p>
The main disadvantage is that templates may cause `<font color=red>code
bloat</font>', since for every combination of parameter values that is
passed to the generic type a new instance of the template is created.
We realize that this may cause problems, but we believe that <font
color=red>with responsible use this problem is rarely
significant</font>.
<p>
The main advantage of templates is that they easily allow a <font
color=red>high-performance implementation</font>.  Since we are
building a compiler for high-performance computing, this is obviously
very attractive.
EOF
&generateFooter($0);
