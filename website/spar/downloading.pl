$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Spar - downloading";

&generate_referal_page( $webpage, $title, "downloading.html" );
