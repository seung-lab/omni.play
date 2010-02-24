#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use File::Basename;
use POSIX;
use Thread;

$SIG{INT} = \&killAllOmnis;

my $numMeshProcessesPerHost = 1; # Set to magic number 1. Don't change.

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

`cd $meshinatorHome; ./`;

sub killAllOmnis {
	`cd $meshinatorHome; ./killAllOmni.pl&`;
}

my $planFile = $argv[1];
my $template = "ssh node onmi headless commandfile parallel project love";

do {
	my $ssh = "ssh ";
	my $node = getCurentNode();
	my $omni = getOmniPath();
	my $headless = "--headless ";

	if (!peekAheadForPendingCommand())
		done();

	my $commandFile = getNextCommandAsFile();
	my $parallel = "";
	my $project = GetProjectFileFromPlanFile($planFile);
	my $love = getLoveFromProject($love);

	projectLove($template);

} while (getNextNode());

printf "done!!\n";

