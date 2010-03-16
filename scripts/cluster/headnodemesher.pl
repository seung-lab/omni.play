#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use File::Basename;
use POSIX;
select STDOUT; $| = 1;


my $start = time ();
my $plan = $ARGV[0];
my $project = substr($plan, 0, length($plan) - length(".plan"));	# If 1 chop off it is for the enter.
my $meshOutputDir = "$ENV{HOME}/meshinator/";
print `mkdir -p $meshOutputDir`;
print `rm -rf $meshOutputDir/data/`;
print `mkdir -p $meshOutputDir`;
print `rsync --inplace $project $plan $meshOutputDir/`;

(my $name, my $path, my $suffix) = fileparse(abs_path($plan));
$plan = "$meshOutputDir/$name";

($name, $path, $suffix) = fileparse(abs_path($project));
$project = "$meshOutputDir/$name";

#print "$plan\n";
#print "$project\n";
#exit(0);

($name, $path, $suffix) = fileparse(abs_path($0));
my $meshinatorHome = $path;

print `cd $meshinatorHome; ./runAcrossCluster.pl \"ls /tmp/failure"\n`;
print "cd $meshinatorHome; ./runAcrossCluster.pl \"rm /tmp/meshinat*\"\n";
print `cd $meshinatorHome; ./runAcrossCluster.pl \"rm /tmp/meshinat\\*\"\n`;
print `cd $meshinatorHome; ./runAcrossCluster.pl \"ls /tmp/meshinat\\*\"\n`;
print "cd $meshinatorHome; ./meshinator.pl $plan --force\n";
print `cd $meshinatorHome; ./meshinator.pl $plan --force`;
print `cd $meshinatorHome; ./runAcrossCluster.pl \"ls /tmp/failure"\n`;
print "cd $meshinatorHome; ./gatherMeshinator.pl $meshOutputDir/data/\n";
print `cd $meshinatorHome; ./gatherMeshinator.pl $meshOutputDir/data/`;
print "find $meshOutputDir -name \"*.h5\" -exec $meshinatorHome/hdf5_merge {} $project \\;\n";
print `find $meshOutputDir -name "*.h5" -exec $meshinatorHome/hdf5_merge {} $project \\;`;
print `/usr/local/bin/du -ach $meshOutputDir`;
print "cd $meshinatorHome; ./runAcrossCluster.pl \"rm /tmp/meshinat*\"\n";
print `cd $meshinatorHome; ./runAcrossCluster.pl \"rm /tmp/meshinat\\*\"\n`;
print `rm -rf $meshOutputDir/data/`;

my $timeSecs = (time() - $start);
print "$timeSecs seconds.\n";
print "meshdone\n";
