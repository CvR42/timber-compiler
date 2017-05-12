$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Spar - inlingin";
&generateHeader( $webpage, $title );
print "<p>\n";
&generate_clickable_img( $webpage, "povray/icon_spar.jpg", "spar/index.html", "spar" );
print <<EOF;
</p>
<p>
Spar supports inlining for two different reasons:
</p>
<ul>
<li>To allow the
abstraction of simple constructs without paying the cost of a function
call.</li>
<li>To allow type abstraction, so that part of
the functionality of C++ templates can be provided.</li>
</ul>
<p>
An inlined method, or constructor is similar to an ordinary one, but it is
declared to be a inlined with the <code>inline</code> keyword. For example:
</p>
<pre>
class Stats {
    long sum;
    int n;

    inline Stats() { sum = 0; n = 0; }
    inline void update( int val ) {
        n++;
        sum += val;
    }
    inline float average() {
        return ((float) val)/((float) n);
    }
}</pre>
<p>
Inlined methods are very similar to ordinary methods, but the compiler
is will expand them at compile time. Moreover, inlined methods can
have <code>type</code> parameters, which is not allowed in ordinary methods.
</p>
<p>
For example, classes such as the <code>DiagonalView</code>
and <code>TransposeView</code> classes shown on the <a
href="array_interface.html">Array interface</a> page usually declare
most of their methods as inlined for increased efficiency.
</p>
EOF
&generateFooter($0);
