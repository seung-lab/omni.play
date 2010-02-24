#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use File::Basename;
use POSIX;

use Thread qw(async);
use Thread::Queue;

my $DataQueue = new Thread::Queue; 
my $howMany = 0;


$SIG{INT} = \&killAllOmnis;

my $maxThreadsToCreate = 300;

my $numMeshProcessesPerHost = 100000000000000000000000000000000000000000000000000000000000000000000000000000000000;

(my $name, my $path, my $suffix) = fileparse( abs_path( $0 ) );
my $meshinatorHome = $path;
my $meshinatorHosts = "$meshinatorHome/hosts";
my $meshinatorOmni = "$meshinatorHome/../../omni/bin/omni";

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


sub killAllOmnis {
    `cd $meshinatorHome; ./killAllOmni.pl&`;
    exit(0);
}

sub byHost {
    $numMeshProcessesPerHost = 1;
    byHostAndProcess();
}

sub byHostAndProcess 
{
    my $maxProcs = $hostCount * $numMeshProcessesPerHost;
    my $chunksPerProccess = 3;
    
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

sub getIdlest()
{
    my $cmdOut = `$meshinatorHome/upTime.pl | sort -rn | tail -n1`;
    my $uptime;
    my $idleNode;
 
    ($uptime, $idleNode) = split (" ", $cmdOut);
    return $idleNode;
}
print "Current idle node is " . getIdlest() . "\n";

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
    my $lockFile = $_[4];
    
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

    print `rm $lockFile`;
}

sub meshinator {
    my $countBackoff = 0;
    my $backoff = 0;
    my @threads;
    for (my $i = 0; $i < $cmdCount;) {
        my $num = $i;
        my $node = $meshCommandHostInput[$i];
        my $outFileName = "$dir/chunk_lists/"."chunks--".$meshCommandHostInput[$i].".$num.txt";
        my $fNameAndPath = $outFileName;
        my $logFile = $outFileName . ".log";
        my $lockFile;
    
        my $j;
        my $found = 0;
        for ($j = 0; $j < $maxThreadsToCreate; $j++) {
           $lockFile = "$dir/$j.lock";
           if (!-e $lockFile) {
              `touch $lockFile`;
              print "$lockFile free\n";
              $found = 1;
              last;
           }
        }
    
        if ($found) {
            $countBackoff++;
	    
            if ($countBackoff > $maxThreadsToCreate) {
                $node = getIdlest();
                print "Sending command to idle node $node.\n";
            }
    	    my $cmd = "ssh $node $meshinatorOmni --headless=$fNameAndPath $projectFile && echo success";
    	    my $thr = new Thread \&runNode, $cmd, $node, $logFile, $fNameAndPath, $lockFile;
    	    push(@threads, $thr);
            $i++;
        } else {
            print "Threads are busy, going to sleep...\n";
            sleep(1);
        }
    }
    foreach my $thread (@threads){ 
        $thread->join;
    }
}

meshinator();

print "done!!\n";
