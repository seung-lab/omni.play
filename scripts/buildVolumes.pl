#!/usr/bin/perl -w

use strict;

my $omniDir = "/home/purcaro/omni.staging";
my $omniExec = $omniDir . "/omni/bin/omni ";
my $scriptFolder = $omniDir . "/scripts/BuildScripts";
my $cmdGeneric = $omniExec." --headless=".$scriptFolder;

sub buildRabbit {
    my $cmd = $cmdGeneric."/buildRabbit.omni.cmd";
    print `$cmd`;
}

sub build150 {
    my $cmd = $cmdGeneric."/build150.omni.cmd";
    print `$cmd`;
}

sub build512 {
    my $cmd = $cmdGeneric."/build512.omni.cmd";
    print `$cmd`;
}

sub menu {
    print "Build volumes menu:\n";
    print "0 -- exit\n";
    print "1 -- Build rabbit\n";
    print "2 -- Build 150^3\n";
    print "3 -- Build 512^3\n";
    print "\n";
    my $max_answer = 3;

    while( 1 ){
	print "Please make selection: ";
	my $answer = <STDIN>;

	if( $answer =~ /^\d+$/ ) {
	    if( ($answer > -1) and ($answer < (1+$max_answer))){
		runBuildVolume( $answer );
		exit();
	    }
	}
    }
}

sub runBuildVolume {
    my $entry = $_[0];

    if( 0 == $entry ){
        return();
    }elsif( 1 == $entry ){
	buildRabbit();
    }elsif( 2 == $entry ){
	build150();
    }elsif( 3 == $entry ){
	build512();
    }
}

sub main {
    menu();
}

main();
