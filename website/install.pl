# File: install.pl
#
# Usage: perl install.pl <installdir> <files>


if( !defined $ARGV[0] ){
    die "No installation directory specified, stopped";
}

$installdir = $ARGV[0];

if( !-e $installdir ){
    die "Installation directory '$installdir' not found, stopped";
}

if( !defined $ARGV[1] ){
    die "No installation directory specified, stopped";
}

shift @ARGV;

#print "installdir: $installdir\n";
#print "files: @ARGV\n";

sub force_directory($)
{
    my ($file) = @_;
    my @chunks = split( '/', $file );
    my $chunk;
    pop @chunks;

    my $path = $installdir;
    foreach $chunk (@chunks){
	$path = "$path/$chunk";
	if( !-d $path ){
	     # The directory does not exist, so try to make it
	     if( -e $path ){
		 die "Cannot make directory '$path', since a normal file is in the way, stopped";
	     }
	     print "mkdir $path\n";
	     system( "mkdir $path" );
	     if( !-d $path ){
		 die "Cannot make directory '$path', stopped";
	     }
	}
    }
}

sub modtime($)
{
    my ($file) = @_;
    if( !-e $file ){
	return 0;
    }
    local ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = stat( $file );
    return $mtime;
}

foreach $file (@ARGV){
    &force_directory( $file );
    my $target = "$installdir/$file";
    if( &modtime( $target )<&modtime( $file ) ){
	print "cp $file $target\n";
	system( "cp $file $target" );
	if( !-e "$target" ){
	    die "Cannot create file '$target', stopped";
	}
    }
}
