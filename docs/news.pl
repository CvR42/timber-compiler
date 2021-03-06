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

&generate_news_item( $webpage, "13 May 2017", "The Timber compiler is noew available on GitHub." ); 

&generate_news_item( $webpage, "13 December 2002", "Version 2.0.1 of the Timber compiler is available for <a href=\"$download_url\">downloading</a>. See the <a href=\"$version_history_url\">version history</a> page for a list of new features." ); 

&generate_news_item( $webpage, "28 March 2002", "Version 2.0 of the Timber compiler is available for <a href=\"$download_url\">downloading</a>. See the <a href=\"$version_history_url\">version history</a> page for a list of new features." ); 

&generate_news_item( $webpage, "21 November 2001", "Version 1.6 of the Timber compiler is available for <a href=\"$download_url\">downloading</a>. See the <a href=\"$version_history_url\">version history</a> page for a list of new features." ); 

&generate_news_item( $webpage, "23 October 2001", "Version 1.5 of the Timber compiler is available for <a href=\"$download_url\">downloading</a>. See the <a href=\"$version_history_url\">version history</a> page for a list of new features." ); 

&generate_news_item( $webpage, "23 October 2001", "A <a href=\"$jacks_url\">separate web page</a> now compares the language conformance of the Timber compiler with that of traditional compilers. The results may surprise you!" );

&generate_news_item( $webpage, "22 March 2001", "Version 1.4 of the Timber compiler is available for <a href=\"$download_url\">downloading</a>. See the <a href=\"$version_history_url\">version history</a> page for a list of new features. Please note that this release requires Kaffe version 1.0.6. This version is <a href=\"$requirements_url\">also available</a> for downloading." );

&generate_news_item( $webpage, "14 Feb 2001", "The website has been overhauled completedly. New features include a more obvious structure of the website, and this news page." );

&generate_news_item( $webpage, "24 Jan 2001", "Version 1.3 of the Timber compiler is available for <a href=\"$download_url\">downloading</a>. See the <a href=\"$version_history_url\">version history</a> page for a list of new features." );

&generateFooter($0);
