#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use File::Basename;
use POSIX;


my $howMany = 0;
my $whoami = `whoami`;
chomp $whoami;


$SIG{INT} = \&killAllOmnis;

my $maxThreadsToCreate = 300;
my $initialPound = 50;

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
    exit(0) if (!-e "/tmp/$whoami.meshinator.kill.lock");
    `rm /tmp/$whoami.meshinator.kill.lock`;
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
    my $chunksPerProccess = 1;
    
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
`rm /tmp/$whoami.meshinator.*.lock`;
`touch /tmp/$whoami.meshinator.kill.lock`;

print "cmdCount = $cmdCount\n";
for (my $i = 0; $i < $cmdCount; $i++) {
    my $num = $i;
    my $outFileName = "$dir/chunk_lists/"."chunks--".$meshCommandHostInput[$i].".$num.txt";
    open OUT_FILE, ">", $outFileName or die "could not write $outFileName";
    print OUT_FILE "parallel:192.168.3.254:8989\n";
    print OUT_FILE $meshCommandChunkInputs[$i];
    close OUT_FILE;
}
my @idleHosts;
sub getIdlest()
{
    if (!scalar @idleHosts) {
       my $cmdOut = `$meshinatorHome/upTime.pl | sort -rn | grep -v Alarm | tail -n10`;
       @idleHosts = split (/\n/, $cmdOut);
    }

    my $theOne = pop @idleHosts;

    my $uptime;
    my $idleNode;
 
    ($uptime, $idleNode) = split (" ", $theOne);
    return $idleNode;
}
print "Current idle node is " . getIdlest() . "\n";

if (-t STDIN) {
    print "proceed to run on cluster? :";
    my $answer = <STDIN>;
    chomp ($answer);
    exit (0) if (uc ($answer) ne "Y");
}
sub doNothing
{
}

sub runNode {
    my $cmd = $_[0];
    my $node = $_[1];
    my $logFile = $_[2];
    my $fNameAndPath = $_[3];
    my $lockFile = $_[4];
    my $pid = fork(); 
    return if ($pid);

    $SIG{INT} = \&doNothing;
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

    print `rm $lockFile`;
    exit(0);
}
my %connectcount;
sub meshinator {
    my $countBackoff = 0;
    my $backoff = 0;
    my @threads;
    my $start = time();

    for (my $i = 0; $i < $cmdCount;) {
        my $num = $i;
        my $node = $meshCommandHostInput[$i];
        my $outFileName = "$dir/chunk_lists/"."chunks--".$meshCommandHostInput[$i].".$num.txt";
        my $fNameAndPath = $outFileName;
        my $logFile = "/tmp/log.$whoami.meshinator.command.$i.log";

        my $lockFile;
    
        my $j;
        my $found = 0;
        for ($j = 0; $j < $maxThreadsToCreate; $j++) {
           $lockFile = "/tmp/$whoami.meshinator.$j.lock";
           if (!-e $lockFile) {
              `touch $lockFile`;
              print "$lockFile free\n";
              $found = 1;
              last;
           }
        }
    
        if ($found) {
            $countBackoff++;
	    
            #if ($countBackoff > $maxThreadsToCreate) {
            if (1) {
                $node = getIdlest();
                print "Sending command to idle node $node.\n";
                next if ($node eq "");
            }
	    $connectcount{$node}++;
    	    my $cmd = "rsh $node $meshinatorOmni --headless=$fNameAndPath $projectFile && echo success";
    	    runNode ($cmd, $node, $logFile, $fNameAndPath, $lockFile);

    	    #my $thr = new Thread \&runNode, $cmd, $node, $logFile, $fNameAndPath, $lockFile;
    	    #push(@threads, $thr);
            $i++;

            my $timeSecs = (time() - $start);
            print "[$i of $cmdCount] " . ($i/$cmdCount * 100) . 
		"% farmed out in $timeSecs seconds ($connectcount{$node}).\n";
            #sleep(1) if ($countBackoff > $initialPound && !($countBackoff > $initialPound *2) );
        } else {
            #print "no lock free?\n";
            sleep(1);
        }
    }
    foreach my $thread (@threads){ 
        $thread->join;
    }
}

meshinator();

print "done!!\n";
