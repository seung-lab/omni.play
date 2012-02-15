#!/usr/bin/perl -w

# move everything but trunk into the branches folder

use strict;

use Tie::File;

die "please specify path" unless 1 == $#ARGV + 1;

my $DIR = $ARGV[0];
my $fileIn = "$DIR/omni.clean.badStructure.dump";
my $fileOut = "$DIR/omni.clean.dump";

my @prefixes = ( "Node-path: ", "Node-copyfrom-path: " );

my @strs = (
    [ "omni/staging/trunk", "branches/staging" ],
    [ "omni/staging",       "branches/staging" ],
    [ "omni/dev",           "branches/dev" ],
    [ "omni/experimental",  "branches/experimental" ],
    [ "omni/trunk",         "trunk" ],
    [ "omni/branches",      "branches" ]
    );

sub rewriteFile 
{
    open my $in,  '<', $fileIn  or die "Can't read old file: $!";
    open my $out, '>', $fileOut or die "Can't write new file: $!";

    # svn dumps are binary files
    binmode $in;
    binmode $out;
    
    while( <$in> )
    {
	my $line = $_;

	foreach (@prefixes)
	{
	    my $prefix = $_; 

	    foreach (@strs)
	    {
		my @pair = $_;
		my $fromStr = $prefix . $pair[0][0];
		my $toStr   = $prefix . $pair[0][1];
		
		$line =~ s/^$fromStr/$toStr/g;
	    }
	}
	
	print $out $line;
    }

    close $out;
}

rewriteFile();
