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
my $createFileLine = "create:".$project."\n";
my $full =  "$project.full";
print `unlink $full`;
my $plan = "$project.plan";

sub runLinesRaw
{
	my $project = "";
	$project = $_[1] if (defined $_[1]);
	open CMD, ">.my.omni.cmd" or die $!;
	print CMD $_[0];
	print `$meshinatorHome/../../omni/bin/omni --headless=.my.omni.cmd $project`;
}

sub runLinesProject
{
	runLinesRaw($_[0], $_[1]);
}

runLinesRaw($createFileLine);

# Add Segmentations
my @h5s = $ARGV[1 .. scalar @ARGV-1];
my $seg = 0;
foreach my $h5 (@h5s)
{
	$seg++;
	my $addSegLines = "addSegmentationFromHDF5:".abs_path($h5)."\n";
 	$addSegLines .= "seg:".$seg."\n";
 	$addSegLines .= "meshplan\n";

	runLinesProject($addSegLines);
	print `cat $plan >> $full`; 
}

print `cat $full > $plan`; 
print `$meshinatorHome/headnodemesher.pl $plan`;

my $timeSecs = time() - $start;
print "$timeSecs seconds.\n";
print "done\n";
