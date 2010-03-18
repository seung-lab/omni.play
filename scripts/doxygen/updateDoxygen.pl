#!/usr/bin/perl -w

use strict;

my $root = "/Users/purcaro/gliamacDoxygen";
my @folders = ("omni.src.dev",
	       "omni.src.stable",
	       "omni.src.staging",
	       "omni.src.trunk" );

foreach (@folders) {
    my $folder = $_;
    my $path = $root."/".$folder;
    
    my $svn_ver = `svnversion $path`;
    $svn_ver =~ s/(.*)\://;
    $svn_ver =~ s/M//;

}


#`perl -i -pe 's/Revision\:(.*)/Revision\:$svn_ver/' Doxyfile`;
#`doxygen`;
