$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Timber - downloading";
&generateHeader( $webpage, $title );
$licence_url = &generate_relative_url( $webpage, "spar/licence.html" );
$requirements_url = &generate_relative_url( $webpage, "requirements.html" );
$changes_url = &generate_relative_url( $webpage, "spar/newfeatures.html" );

print <<EOF;

<h2>Platform</h2>

<p>
The Timber compiler has been tested extensively on Linux systems with
gcc 2.95 and gcc 3.0, and is also known to work on Windows NT 4.0 with
the <a href="http://www.cygwin.com/">CygWin</a> compiler and toolset.
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

<h2>Licence information</h2>

<p>
Spar is covered by the GNU copyleft licence. See the <a
href="$licence_url">Spar licence page</a> for details.
</p>

<h2>Downloading and unpacking the Timber compiler</h2>
<p>
You can download the most recent version of Timber compiler (release $spar_version, released $spar_release_date) here:
</p>
EOF
&generate_downloads( $webpage, "spar/timber-$spar_version" );

if( defined( $spar_pre_version ) ){
    print <<EOF;
<p>
A pre-release version of the compiler is also available.
This version is not as thoroughly tested as the version above.
</p>
EOF
    &generate_downloads( $webpage, "spar/timber-$spar_pre_version" );
}

print <<EOF;

<p>
See the <a href="$changes_url">version history</a> page for an
overview of the change history of the Timber compiler.
</p>

<h2>Requirements</h2>
<p>
See the <a href="$requirements_url">requirements</a> page for
further information on the requirements of the Timber compiler.
</p>

<h2>Unpacking, compiling and installing</h2>

<p>
The compiler is packaged in a compressed <tt>tar</tt> file, which 
you can unpack with the command
</p>

<pre>
tar xvfz timber-$spar_version.tar.gz
</pre>

<p>
This will create a directory <tt>timber-$spar_version</tt>.
In the unpacked directory there is a README file that describes how
to compile and install the package.
</p>

<h2>Documentation</h2>
<p>
You can download the Spar Language Specification here:
</p>
EOF
&generate_downloads( $webpage, "spar/spar-langspec-$spar_langspec_version" );

if( defined( $spar_langspec_pre_version ) ){
    print <<EOF;
<p>
A pre-release version of the Spar Language Specification is also available.
</p>
EOF
    &generate_downloads( $webpage, "spar/spar-langspec-$spar_langspec_pre_version" );
}


&generateFooter($0);
