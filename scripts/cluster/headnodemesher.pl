#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use File::Basename;
use POSIX;
use Thread;

my $numMeshProcessesPerHost = 8;

(my $name, my $path, my $suffix) = fileparse( abs_path( $0 ) );
my $meshinatorHome = $path;
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

sub byHost {
    my $chunksPerHost = ceil( $chunkCount / $hostCount );
    
    print "$chunkCount chunks with chunksPerHost = $chunksPerHost\n";
    print "$hostCount is hostCount\n";
    
    my $count = 0;
    my $chunkBatch;
    $cmdCount = 0;
    my $nodeCount = 0;
    while ($count < $chunkCount) {
	my $chunk = $chunks[$count];
	$chunkBatch .= $chunk;
	$count++;
	if (0 eq $count % $chunksPerHost) {
	    $meshCommandChunkInputs[$cmdCount] = $chunkBatch;
	    $meshCommandHostInput[$cmdCount] = $hostList[ $nodeCount ];
	    $nodeCount++;
	    chop $meshCommandHostInput[$cmdCount];
	    $cmdCount++;
	    $chunkBatch = "";
	}
    }

    if ($chunkBatch ne "") {
	print $chunkBatch;
	$meshCommandChunkInputs[$cmdCount-1] .= $chunkBatch;
    }    

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
	$meshCommandChunkInputs[$cmdCount-1] .= $chunkBatch;
    }
}

#byHost();
byHostAndProcess();

`mkdir -p chunk_lists`;
`rm -f chunk_lists/*`;

print "cmdCount = $cmdCount\n";
for (my $i = 0; $i < $cmdCount; $i++) {
    my $num = $i;
    my $outFileName = "chunk_lists/"."chunks--".$meshCommandHostInput[$i].".$num.txt";
    open OUT_FILE, ">", $outFileName or die "could not write $outFileName";
    print OUT_FILE $meshCommandChunkInputs[$i];
    close OUT_FILE;
}

($name, $path, $suffix) = fileparse( abs_path( $ARGV[0] ) );
my $dir = $path;
my $projectFile = $path . $name;
$projectFile =~ s/\.plan$//;

sub runNode 
{
    my $cmd = $_[0];
    my $node = $_[1];

    my $start = time();
    print "node $node: starting meshing...\n"; 
    `$cmd`;
    my $end = time();
    my $timeSecs = ($end - $start);
    print "node $node: done meshing (".$timeSecs." seconds)\n";
}

my @threads;
for (my $i = 0; $i < $cmdCount; $i++) {
    my $num = $i;
    my $node = $meshCommandHostInput[$i];
    my $outFileName = "chunk_lists/"."chunks--".$meshCommandHostInput[$i].".$num.txt";
    my $fNameAndPath = $dir . $outFileName;
    my $logFile = $outFileName + ".log";
    my $cmd = "ssh $node /home/purcaro/omni.staging/omni/bin/omni --headless=$fNameAndPath $projectFile > $logFile";
    my $thr = new Thread \&runNode, $cmd, $node;
    push(@threads, $thr);
}

foreach my $thread (@threads){ 
    $thread->join;
}

print "done!!\n";
