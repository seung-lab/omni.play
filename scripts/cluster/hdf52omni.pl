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
chomp($project);
my $createFileLine = "create:".$project."\n";

print "will create \"$createFileLine\"\n";

my $full =  "$project.full";
print `unlink $full`;
my $plan = "$project.plan";

sub runLinesRaw
{
	my $project = "";
	$project = $_[1] if (defined $_[1]);
	open CMD, ">.my.omni.cmd" or die $!;
	print CMD $_[0];
	print "running; $meshinatorHome/../../omni/bin/omni --headless=.my.omni.cmd $project";
	print `$meshinatorHome/../../omni/bin/omni --headless=.my.omni.cmd $project`;
}

sub runLinesProject
{
	runLinesRaw($_[0], $_[1]);
}

#runLinesRaw($createFileLine);

# Add Segmentations
my @h5s = @ARGV[1 .. scalar @ARGV-1];
#print @h5s;
#exit(0);
my $seg = 0;
foreach my $h5 (@h5s)
{
	$seg++;
	my $addSegLines = $createFileLine;
	$addSegLines .= "addSegmentationFromHDF5:".abs_path($h5)."\n";
 	$addSegLines .= "seg:".$seg."\n";
 	$addSegLines .= "meshplan\n";

	runLinesProject($addSegLines);
	print `cat $plan >> $full`; 
}

exit(0);

print `cat $full > $plan`; 
print `$meshinatorHome/headnodemesher.pl $plan`;

my $timeSecs = time() - $start;
print "$timeSecs seconds.\n";
print "done\n";
