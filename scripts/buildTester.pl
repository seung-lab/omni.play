#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use File::Basename;
use POSIX;

(my $scriptName, my $scriptPath, my $scriptSuffix) = fileparse( abs_path( $0 ) );

if (2 != @ARGV) {
	usage() && die;
}

my $inomnifile = $ARGV[0];
my $inh5file = $ARGV[1];

print `cp $inomnifile $inomnifile.test`;

my $omnicmdfiletext = <<__HERE__;
loadHDF5seg:$inh5file
loadHDF5chann:$inh5file
compareSegs:1,2
compareChanns:1,2
__HERE__

open CMD, ">$scriptPath/.omni.build.test.cmd" or die $!;
print CMD $omnicmdfiletext;
close CMD;

print "$scriptPath/../omni/bin/omni --headless=$scriptPath/.omni.build.test.cmd $inomnifile.test";
print `$scriptPath/../omni/bin/omni --headless=$scriptPath/.omni.build.test.cmd $inomnifile.test`;
print `rm  $inomnifile.test`;

sub usage
{
	print "please call buildTester.pl like: buildTester.pl known.omni known.h5\n"
}
