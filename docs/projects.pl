$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Projects related to Timber";
&generateHeader( $webpage, $title );
&generate_img( $webpage, "povray/icon_timber.jpg", "" );
print <<EOF;
<p>
EOF
&generate_under_construction($webpage);
&generateFooter($0);
