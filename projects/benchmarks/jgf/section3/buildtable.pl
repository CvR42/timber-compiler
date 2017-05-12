@benchmarks = (
    "JGFEulerBenchSizeA",
    "JGFEulerBenchSizeB",
    "JGFMolDynBenchSizeA",
    "JGFMolDynBenchSizeB",
    "JGFMonteCarloBenchSizeA",
    "JGFMonteCarloBenchSizeB",
    "JGFRayTracerBenchSizeA",
    "JGFRayTracerBenchSizeB",
    "JGFSearchBenchSizeA",
    "JGFSearchBenchSizeB",
);

@variants = (
    "java-b1n1",
    "java-b0n1",
    "java-b1n0",
    "java-b0n0",
    "hotspot-desktop",
    "hotspot-server",
);

$oldbnm = "";
foreach $bm (@benchmarks){
    $bm1 = $bm;
    $bm1 =~ s/JGF(.*)BenchSize(.)/\1 \& \2/;
    $bnm = $1;
    $bsz = $2;
    if( $oldbnm eq $bnm ){
	print "       & $bsz";
    }
    else {
	print "\\hline\n";
	print "$bnm & $bsz";
    }
    $oldbnm = $bnm;
    foreach $v (@variants){
	$fn = "$bm-$v.time";
	if( -e $fn ){
	    $val = eval `cat $fn`;
	    printf " & %4.1f", $val;
	}
	else {
	    print " &  --  ";
	}
    }
    print " \\\\\n";
}
