$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Timber - requirements";
&generateHeader( $webpage, $title );
$licence_url = &generate_relative_url( $webpage, "spar/licence.html" );
$changes_url = &generate_relative_url( $webpage, "spar/newfeatures.html" );

print <<EOF;

<h2>Platform</h2>

<p>
The Timber compiler has been tested extensively on Linux systems with
gcc 2.95.2, and is also known to work on Windows NT 4.0 with the <a
href="http://www.cygwin.com/">CygWin</a> compiler and
toolset.
</p>

<p>
The Timber compiler is intended to be portable to a large number of
platforms. Until now we have only tested it on the platforms mentioned
above, but we wouldn't be surprised if it would work on other platforms,
perhaps with a few patches. Any reports on the success or failure on
other platforms are welcome. Patches will be gratefully accepted.
</p>

<p>
For the moment only the GNU compiler is supported, but this is only
because the configuration scripts are not yet smart enough to generate
the correct options for other compilers. We hope to correct that
restriction soon.
</p>

<h2>Requirements</h2>

<p>
To be able to compile the Timber compiler, you must have <a
href="$tm_url">Tm</a>
installed (at least version $tm_min_version is required). Make sure that the
directory where the program 'tm' is installed is in your search
path. (In other words, you must be able to invoke 'tm' without prefixing
the full path to the program.)
You can clone and compile Tm from the <a href="$tm_url">Tm github site</a>.
</p>
<p>
Also, the Timber compiler requires an implementations of the functions in the
Java standard library.  We use the implementation provided by <a
href="http://www.kaffe.org">Kaffe</a>. In particular, we require `Desktop'
version 1.0.6 of the Kaffe library. Unfortunately this version is so
old that it is no longer available from the Kaffe website, but you
can download a copy from our website:
</p>

EOF
&generate_downloads( $webpage, "spar/kaffe-1.0.6" );

print <<EOF;

<p>
The Timber compiler requires that the environment variable <tt>KAFFEROOT</tt>
points to the directory where you have installed Kaffe. For example,
if you unpacked the Kaffe distribution in your home directory, you would
set the <tt>KAFFEROOT</tt> environment variable like this:
</p>

<pre>
setenv KAFFEROOT \$HOME/kaffe-1.0.6</pre>

<p>
If you are using a Bourne-like shell such as <tt>sh</tt> or <tt>bash</tt>,
the command <tt>setenv</tt> is not available, and you must use the
<tt>export</tt> command. For example:
</p>

<pre>
export KAFFEROOT=\$HOME/kaffe-1.0.6</pre>
 
EOF

&generateFooter($0);
