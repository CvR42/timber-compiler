$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "News about the Timber compiler";
&generateHeader( $webpage, $title );

$version_history_url = &generate_relative_url( $webpage, "spar/newfeatures.html" );
$download_url = &generate_relative_url( $webpage, "downloading.html" );
$jacks_url = &generate_relative_url( $webpage, "jacks.html" );
$requirements_url = &generate_relative_url( $webpage, "requirements.html" );
print <<EOF;
<h2>News items</h2>

EOF

&generate_news_item( $webpage, "13 December 2002", "Version 2.0.1 of the Timber compiler is available for <a href=\"$download_url\">downloading</a>. See the <a href=\"$version_history_url\">version history</a> page for a list of new features." ); 

&generate_news_item( $webpage, "28 March 2002", "Version 2.0 of the Timber compiler is available for <a href=\"$download_url\">downloading</a>. See the <a href=\"$version_history_url\">version history</a> page for a list of new features." ); 

&generate_news_item( $webpage, "21 November 2001", "Version 1.6 of the Timber compiler is available for <a href=\"$download_url\">downloading</a>. See the <a href=\"$version_history_url\">version history</a> page for a list of new features." ); 

&generate_news_item( $webpage, "23 October 2001", "Version 1.5 of the Timber compiler is available for <a href=\"$download_url\">downloading</a>. See the <a href=\"$version_history_url\">version history</a> page for a list of new features." ); 

&generate_news_item( $webpage, "23 October 2001", "A <a href=\"$jacks_url\">separate web page</a> now compares the language conformance of the Timber compiler with that of traditional compilers. The results may surprise you!" );

&generate_news_item( $webpage, "22 March 2001", "Version 1.4 of the Timber compiler is available for <a href=\"$download_url\">downloading</a>. See the <a href=\"$version_history_url\">version history</a> page for a list of new features. Please note that this release requires Kaffe version 1.0.6. This version is <a href=\"$requirements_url\">also available</a> for downloading." );

&generate_news_item( $webpage, "14 Feb 2001", "The website has been overhauled completedly. New features include a more obvious structure of the website, and this news page." );

&generate_news_item( $webpage, "24 Jan 2001", "Version 1.3 of the Timber compiler is available for <a href=\"$download_url\">downloading</a>. See the <a href=\"$version_history_url\">version history</a> page for a list of new features." );

print <<EOF;
<h2>Timber announcements mailing list</h2>
<p>
If you want to keep informed about the development of the Timber 
compiler, you may want to subscribe to the announcements mailing list.
This is a low-volume mailing list to announce updates of the Timber
compiler.
</p>
<p>
You can subscribe or unsubscribe to the mailing list by
clicking one of the buttons below:
</p>
 
<form action="mailto:$timberAnnounceRequestEmailAddress" method="post" enctype="text/plain">
<p> 
<input type="submit" name="subscribe " value=" Subscribe" />
<input type="submit" name="unsubscribe " value=" Unsubscribe" />
</p>
</form>
<p>
Note: this does not work on all web browsers, and may require the
installation of additional software.
</p>
<p> 
Alternatively, send an email with the subject <tt>subscribe</tt>
or <tt>unsubscribe</tt> to the email address <a
href="mailto:$timberAnnounceRequestEmailAddress?subject=subscribe">$timberAnnounceRequestEmailAddress</a>.
</p>

EOF

&generateFooter($0);
