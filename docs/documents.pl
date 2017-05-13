$webpage = $0;
$webpage =~ s/\.pl//;
require 'style.pli';
$title = "Documents about the Timber compiler";
&generateHeader( $webpage, $title );

print <<EOF;

<h2>Selected publications</h2>

EOF

&generate_conferencepaper_reference(
  $webpage,
  "C. van Reeuwijk",
  "Rapid and Robust Compiler Construction Using Template-Based Metacompilation",
  "CC2003",
  "(to be determined)",
  "Warsaw, Poland",
  "April 2003",
  "publications/cc-2003.pdf"
);

&generate_article_reference(
  $webpage,
  "C. van Reeuwijk, F. Kuijlman and H.J. Sips",
  "Spar: a set of extensions to Java for scientific computation",
  "Concurrency and Computation: Practice and Experience",
  "15(3-5)",
  "277-299",
  "2003",
  "publications/cpe-jgrande2001.pdf"
);

&generate_conferencepaper_reference(
  $webpage,
  "C. van Reeuwijk and H.J. Sips",
  "Adding Tuples to Java: a Study in Lightweight Data Structures",
  "Proc. of the Java Grande/ISCOPE 2002 Conference",
  "(to be determined)",
  "Seattle",
  "November 2002",
  "downloads/reeuwijk-jgi2002-tuples.pdf"
);

&generate_conferencepaper_reference(
  $webpage,
 "F. Kuijlman, C. van Reeuwijk, W.J.A. Denissen, and H.J. Sips",
 "A Unified Compiler Framework for Work and Data Placement",
 "Proceedings of the 9th International Conference on Compilers for Parallel Computers (CPC2001)",
 "259-268",
 "Edinburgh, Scotland",
 "June 2001"
);

&generate_conferencepaper_reference(
  $webpage,
  "C. van Reeuwijk, F. Kuijlman, H.J. Sips",
  "Annotating Spar/Java for data-parallel programming",
  "Proceedings of the Sixth Annual Conference of the Advanced Scool for Computing and Imaging",
  "185-194",
  "Lommel, Belgium",
  "June 2000",
  "publications/asci2000.ps.gz"
);


&generate_conferencepaper_reference(
  $webpage,
  "F. Kuijlman, C. van Reeuwijk, A.J.C. van Gemund, and H.J. Sips",
  "Code Generation Techniques for the Task-Parallel Language Spar",
  "Proceedings of the 7th International Conference on Compilers for Parallel Computers (CPC'98)",
  "1-11",
  "Linkoping, Sweden",
  "June 1998"
);

&generate_article_reference(
  $webpage,
  "Kees van Reeuwijk, Arjan J.C. van Gemund, and Henk J. Sips",
  "Spar: A Programming Language for Semi-automatic Compilation of Parallel Programs",
  "Concurrency - Practice and Experience",
  "11(9)",
  "1193-1205",
  "November 1997",
  "publications/cpe-1997.pdf"
);

&generate_article_reference(
  $webpage,
  "Kees van Reeuwijk, Will Denissen, Henk J. Sips and Edwin M. R. M. Paalvast",
  "An Implementation framework for HPF distributed arrays on message-passing parallel computer systems",
  "IEEE Transactions on Parallel and Distributed Systems",
  "7(9)",
  "897-914",
  "September 1996",
  "publications/ieee-tpds-1996.pdf"
);
print <<EOF;
<h3>Language specification</h3>
<p>
The Spar language specification has not been formally published, but it is
available for download:
</p>
EOF

&generate_downloads( $webpage, "spar/spar-langspec-$spar_langspec_version" );

print <<EOF;

<h3>Presentations</h3>
<p>
Overhead sheets of the DAS technical meeting of 30 november 1999.
This 20-minute presentation gives an overview of the Timber project, 
and some preliminary measurements for parallel execution on the DAS cluster.
</p>
EOF
&generate_downloads( $webpage, "presentations/das-30nov99" );
&generateFooter($0);
