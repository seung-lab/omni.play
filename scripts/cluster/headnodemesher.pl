#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use File::Basename;
use POSIX;
use Thread;

$plan = $ARGV[0];
$project = substr($plan, 0, length($plan) - length(".plan"));	# If 1 chop off it is for the enter.
$meshOutputDir = "/Users/$ENV{HOME}/meshinator/";
print `mkdir -p $meshOutputDir`;

(my $name, my $path, my $suffix) = fileparse(abs_path($0));
my $meshinatorHome = $path;
print `cd $meshinatorHome; ./runAcrossCluster.pl rm -rf /tmp/meshinator*`;
print `cd $meshinatorHome; ./$meshinatorHome/meshinator.pl $plan`;
print `cd $meshinatorHome; ./gatherMeshinator.pl $meshOutputDir`;
print `find $meshOutputDir -exec $meshinatorHome/hdf5_merge {} $project`;

exit(0);

my $meshinatorHosts = "$meshinatorHome/hosts";

open HOSTS, "$meshinatorHosts" or die "could not find $meshinatorHosts";
my @hostList = <HOSTS>;
close HOSTS;
my $hostCount = scalar @hostList;

open CHUNKS, $ARGV[0] or die $!;
my @chunks = <CHUNKS>;
close CHUNKS;
my $chunkCount = scalar @chunks;

my $cmdCount = 0;
my @meshCommandChunkInputs;
my @meshCommandHostInput;

`cd $meshinatorHome; ./`;


sub byHost {
    $numMeshProcessesPerHost = 1;
    byHostAndProcess();
}

sub byHostAndProcess 
{
    my $maxProcs = $hostCount * $numMeshProcessesPerHost;
    my $chunksPerProccess = $chunkCount / $maxProcs;
    $chunksPerProccess = 1.0 if ($chunksPerProccess lt 1.0);
    
    print "$chunkCount chunks with chunksPerProccess = $chunksPerProccess\n";
    print "$hostCount is hostCount\n";
    
    my $count = 0;
    my $chunkBatch;
    $cmdCount = 0;
    while ($count < $chunkCount) {
	my $chunk = $chunks[$count];
	$chunkBatch .= $chunk;
	$count++;
	if (0 eq $count % $chunksPerProccess) {
	    $meshCommandChunkInputs[$cmdCount] = $chunkBatch;
	    $meshCommandHostInput[$cmdCount] = $hostList[$cmdCount % $hostCount];
	    chop $meshCommandHostInput[$cmdCount];
	    $cmdCount++;
	    $chunkBatch = "";
	}
    }
    if ($chunkBatch ne "") {
	my @marginals = split (/\n/, $chunkBatch);
	my $margcount = 0;
	foreach my $marg (@marginals) {
	   $meshCommandChunkInputs[$margcount % $cmdCount] .= "$marg\n";
	   $margcount++;
	}
	$meshCommandChunkInputs[$cmdCount-1] .= $chunkBatch;
    }
}

#byHost();
byHostAndProcess();

($name, $path, $suffix) = fileparse( abs_path( $ARGV[0] ) );
my $dir = $path;
my $projectFile = $path . $name;
$projectFile =~ s/\.plan$//;

`rm -rf $dir/chunk_lists`;
`mkdir -p $dir/chunk_lists/`;

print "cmdCount = $cmdCount\n";
for (my $i = 0; $i < $cmdCount; $i++) {
    my $num = $i;
    my $outFileName = "$dir/chunk_lists/"."chunks--".$meshCommandHostInput[$i].".$num.txt";
    open OUT_FILE, ">", $outFileName or die "could not write $outFileName";
    print OUT_FILE "parallel:192.168.3.254:8989\n";
    print OUT_FILE $meshCommandChunkInputs[$i];
    close OUT_FILE;
}

if (-t STDIN) {
    print "proceed to run on cluster? :";
    my $answer = <STDIN>;
    chomp ($answer);
    exit (0) if (uc ($answer) ne "Y");
}

sub runNode {
    my $cmd = $_[0];
    my $node = $_[1];
    my $logFile = $_[2];
    my $fNameAndPath = $_[3];
    
    my $start = time();
    print "node $node: starting meshing...\n"; 
    my $result = `$cmd`;
    #print $cmd."\n";

    open OUT_FILE, ">", $logFile or die "could not write $logFile";
    print OUT_FILE $result;
    close OUT_FILE;

    my $end = time();
    my $timeSecs = ($end - $start);
    my $chunkCoord = `tail -n 1 $fNameAndPath`;
    chomp( $chunkCoord );
    print "node $node: done meshing $chunkCoord (".$timeSecs." seconds)\n";
}

my @threads;
for (my $i = 0; $i < $cmdCount; $i++) {
    my $num = $i;
    my $node = $meshCommandHostInput[$i];
    my $outFileName = "$dir/chunk_lists/"."chunks--".$meshCommandHostInput[$i].".$num.txt";
    my $fNameAndPath = $outFileName;
    my $logFile = $outFileName . ".log";
    my $cmd = "ssh $node $meshinatorOmni --headless=$fNameAndPath $projectFile && echo success";
    my $thr = new Thread \&runNode, $cmd, $node, $logFile, $fNameAndPath;
    push(@threads, $thr);
}

foreach my $thread (@threads){ 
    $thread->join;
}

print "done!!\n";
