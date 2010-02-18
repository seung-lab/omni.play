#!/usr/bin/perl -w

use strict;

my $numMeshProcessesPerHost = 2;
my $meshinatorHome = $ENV{HOME};
my $meshinatorHosts = "$meshinatorHome/omnimeshhosts.txt";

open HOSTS, "$meshinatorHosts" or die $!;
my @hostList = <HOSTS>;
my $hostCount = scalar @hostList;
open CHUNKS, $ARGV[0] or die $!;
my @chunks = <CHUNKS>;
my $chunkCount = scalar @chunks;

my $maxProcs = $hostCount * $numMeshProcessesPerHost;
my $chunksPerProccess = $chunkCount / $maxProcs;
$chunksPerProccess = 1.0 if ($chunksPerProccess lt 1.0);

print "$chunkCount chunks with chunksPerProccess = $chunksPerProccess\n";
print "$hostCount is hostCount\n";

my $count = 0;
my $chunkBatch;
my $cmdCount = 0;
my @meshCommandChunkInputs;
my @meshCommandHostInput;
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
	$meshCommandChunkInputs[$cmdCount] .= $chunkBatch;
}

print "cmdCount = $cmdCount\n";
for (my $i = 0; $i < $cmdCount; $i++) {
	print "Host: " . $meshCommandHostInput[$i]  . " gets...\n" . $meshCommandChunkInputs[$i];
}
