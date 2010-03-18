#!/usr/bin/perl -w

use strict;
use XML::Simple;

my $root = "/Users/purcaro/gliamacDoxygen";
my $main_output = $root."/www/doxygenOutput";
my @folders = ("omni.src.dev",
	       "omni.src.stable",
	       "omni.src.staging",
	       "omni.src.trunk" );

foreach (@folders) {
    my $folder = $_;

    my $path = $root."/".$folder;   

    `svn up $path`;

    my $xml_log = `svn log -l 1 --with-no-revprops --xml $path`;
    my $xs1 = XML::Simple->new();
    my $doc = $xs1->XMLin($xml_log);
    my $svn_ver = $doc->{logentry}->{revision};

    print $path." (".$svn_ver.")\n";

    my $output_folder = $main_output."/".$folder."/".$svn_ver;
    `mkdir -p $output_folder`;

    
}


#`perl -i -pe 's/Revision\:(.*)/Revision\:$svn_ver/' Doxyfile`;
#`doxygen`;
