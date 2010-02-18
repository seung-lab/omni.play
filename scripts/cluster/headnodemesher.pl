#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use File::Basename;
use POSIX;

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
