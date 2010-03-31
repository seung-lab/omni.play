#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use File::Basename;
use POSIX;
select STDOUT; $| = 1;

my $start = time ();
my ($name, $path, $suffix) = fileparse(abs_path($0));
my $meshinatorHome = $path;

# Create File
my $project = abs_path($ARGV[0]);
my $createFileLine = "createOrOpen:".$project."\n";
my $full =  "$project.full";
print `rm -f $full`;
my $plan = "$project.plan";

# Add Segmentations
my @h5s = @ARGV[1 .. scalar @ARGV-1];
foreach my $h5 (@h5s)
{
	my $addSegLines = $createFileLine;

	my $hf5path = abs_path($h5);
	print "about to build $hf5path..\n";
	$addSegLines .= "addSegmentationFromHDF5:".$hf5path."\n";

        # seg:number is already set by the time we get here
 	$addSegLines .= "meshplan\n";  

	# runLinesProject($addSegLines);

	`cat $addSegLines > .my.omni.cmd`;

	my $cmd = "$meshinatorHome/../../omni/bin/omni --headless=.my.omni.cmd $project";
	print "running ($cmd)\n";
	print `$cmd`;

	print `cat $plan >> $full`; 
}

exit(0);

print `cat $full > $plan`; 
print `$meshinatorHome/headnodemesher.pl $plan`;

my $timeSecs = time() - $start;
print "$timeSecs seconds.\n";
print "done\n";
