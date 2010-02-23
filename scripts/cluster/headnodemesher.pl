#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use File::Basename;
use POSIX;
use Thread;

$SIG{INT} = \&killAllOmnis;

my $numMeshProcessesPerHost = 2;

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


sub killOmniOnNode {
    my $node = $_[0];
    `ssh ${node} killall -9 omni`;
}

sub killAllOmnis {
    open IN_FILE,  "<", "hosts"  or die "could not read hosts";

    my @threads;
    while (my $line = <IN_FILE>) {
	chomp( $line );
	my $thr = new Thread \&runNode, $line;
	push(@threads, $thr);
    }
    close IN_FILE;

    foreach my $thread (@threads){ 
	$thread->join;
    }
}

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
	$meshCommandChunkInputs[$cmdCount-1] .= $chunkBatch;
    }
}

#byHost();
byHostAndProcess();

($name, $path, $suffix) = fileparse( abs_path( $ARGV[0] ) );
my $dir = $path;
my $projectFile = $path . $name;
$projectFile =~ s/\.plan$//;

`mkdir -p $dir/chunk_lists`;
`rm -f $dir/chunk_lists/*`;

print "cmdCount = $cmdCount\n";
for (my $i = 0; $i < $cmdCount; $i++) {
    my $num = $i;
    my $outFileName = "$dir/chunk_lists/"."chunks--".$meshCommandHostInput[$i].".$num.txt";
    open OUT_FILE, ">", $outFileName or die "could not write $outFileName";
    print OUT_FILE "parallel:192.168.3.254:8989\n";
    print OUT_FILE $meshCommandChunkInputs[$i];
    close OUT_FILE;
}

sub runNode {
    my $cmd = $_[0];
    my $node = $_[1];
    my $logFile = $_[2];

    my $start = time();
    print "node $node: starting meshing...\n"; 
    my $result = `$cmd`;
    #print $cmd."\n";

    open OUT_FILE, ">", $logFile or die "could not write $logFile";
    print OUT_FILE $result;
    close OUT_FILE;

    my $end = time();
    my $timeSecs = ($end - $start);
    print "node $node: done meshing (".$timeSecs." seconds)\n";
}

my @threads;
for (my $i = 0; $i < $cmdCount; $i++) {
    my $num = $i;
    my $node = $meshCommandHostInput[$i];
    my $outFileName = "$dir/chunk_lists/"."chunks--".$meshCommandHostInput[$i].".$num.txt";
    my $fNameAndPath = $outFileName;
    my $logFile = $outFileName . ".log";
    my $lockFile = $outFileName . ".lock";
    my $cmd = "ssh $node sleep 1 && link $projectFile $lockFile && stat -t $lockFile && /home/purcaro/omni.staging/omni/bin/omni --headless=$fNameAndPath $projectFile && echo success";
    my $thr = new Thread \&runNode, $cmd, $node, $logFile;
    push(@threads, $thr);
}

foreach my $thread (@threads){ 
    $thread->join;
}

print "done!!\n";
