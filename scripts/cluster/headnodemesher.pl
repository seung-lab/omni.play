#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use File::Basename;
use POSIX;
select STDOUT; $| = 1;


my $plan = $ARGV[0];
my $project = substr($plan, 0, length($plan) - length(".plan"));	# If 1 chop off it is for the enter.
my $meshOutputDir = "$ENV{HOME}/meshinator/";
print `rm -rf $meshOutputDir/`;
print `mkdir -p $meshOutputDir`;

(my $name, my $path, my $suffix) = fileparse(abs_path($0));
my $meshinatorHome = $path;

print `cd $meshinatorHome; ./runAcrossCluster.pl \"ls /tmp/failure"\n`;
print "cd $meshinatorHome; ./runAcrossCluster.pl \"rm /tmp/meshinat*\"\n";
print `cd $meshinatorHome; ./runAcrossCluster.pl \"rm /tmp/meshinat\\*\"\n`;
print `cd $meshinatorHome; ./runAcrossCluster.pl \"ls /tmp/meshinat\\*\"\n`;
print "cd $meshinatorHome; ./meshinator.pl $plan --force\n";
print `cd $meshinatorHome; ./meshinator.pl $plan --force`;
print `cd $meshinatorHome; ./runAcrossCluster.pl \"ls /tmp/failure"\n`;
print "cd $meshinatorHome; ./gatherMeshinator.pl $meshOutputDir\n";
print `cd $meshinatorHome; ./gatherMeshinator.pl $meshOutputDir`;
print "find $meshOutputDir -exec $meshinatorHome/hdf5_merge {} $project \\;\n";
print `find $meshOutputDir -exec $meshinatorHome/hdf5_merge {} $project \\;`;


