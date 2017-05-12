$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Language compatibility";
&generateHeader( $webpage, $title );
$jacksurl = "http://www-124.ibm.com/developerworks/oss/cvs/jikes/~checkout~/jacks/jacks.html";

print <<EOF;

<p>
Timber is also an effective static Java compiler. To evaluate the
compatibility with the Java language specification, we have used
the <a href="$jacksurl">Jacks validation suite</a>. The table below
lists the results for a number of compilers.
</p>

<p>
<table border="1">
<caption><em>Results of the Jacks validation suite for various compilers</em></caption>
<tr><th>Compiler</th><th>Date</th><th>Total</th><th>Passed</th><th>Skipped</th><th>Failed</th></tr>
<tr align="right"><th>Kaffe</th><td align="center">(1)</td><td>1840</td><td>1420</td><td>8</td><td>412</td></tr>
<tr align="right"><th>GCJ</th><td align="center">(1)</td><td>1841</td><td>1459</td><td>40</td><td>342</td></tr>
<tr align="right"><th>Sun JDK 1.3</th><td></td><td>2064</td><td>1869</td><td>16</td><td>179</td></tr>
<tr align="right"><th>Sun JDK 1.4 beta 2</th><td>20 november 2001</td><td>2247</td><td>2049</td><td>17</td><td>181</td></tr>
<tr align="right"><th>Timber 1.6</th><td>21 november 2001</td><td>2247</td><td>2027</td><td>49</td><td>171</td></tr>
<tr align="right"><th>Jikes</th><td align="center">(1)</td><td>2929</td><td>2637</td><td>100</td><td>192</td></tr>
</table>
</p>

<p>Notes:</p>
<ol>
<li>Results dated as (1) were extracted from the log files in the Jacks
distribution.  The exact compiler version and Jacks version that were
used are not
known. Since Jacks is part of the Jikes compiler project, presumably
the Jikes version is always recent.</li>
<li>The Timber compiler was run with the <code>--verify</code> and the
<code>--strictanalysis</code> options. The <code>--verify</code> option
forces all code in the given source file to be compiled (normally Timber
tries to compile only what is needed for the <code>main</code> method).
The <code>--strictanalysis</code> option tells the compiler not to be
smarter than a standard Java compiler when doing used-before-set analysis.
Since the programs in the validation suite have the <code>.java</code>
extension, Spar language constructs are not recognized.</li>
<li>The Timber compiler does not implement the at-file command-line construct
that isn't documented in the Java Language Specification, but is
implemented in Sun's compilers, and copied by a number of others.
This accounts for 25 tests skipped by Timber.</li>
</ol>
EOF

&generateFooter($0);
