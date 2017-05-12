@benchmarks = (
    "JGFLUFactSparBenchSizeA",
    "JGFLUFactSparBenchSizeB",
    "JGFLUFactSparBenchSizeC",
    "JGFSORSparBenchSizeA",
    "JGFSORSparBenchSizeB",
    "JGFSORSparBenchSizeC",
    "JGFSeriesSparBenchSizeA",
    "JGFSeriesSparBenchSizeB",
    "JGFSeriesSparBenchSizeC",
);

@variants = (
    "java-b1n1",
    "java-b0n1",
    "java-b1n0",
    "java-b0n0",
);

print "\\hline\n";
$oldbnm = "";
foreach $bm (@benchmarks){
    $bm1 = $bm;
    $bm1 =~ s/JGF(.*)BenchSize(.)/\1 \& \2/;
    $bnm = $1;
    $bsz = $2;
    if( $oldbnm eq $bnm ){
	print " & $bsz";
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
	    printf " & %3.1f", $val;
	}
	else {
	    print " & - ";
	}
    }
    print " \\\\\n";
}
