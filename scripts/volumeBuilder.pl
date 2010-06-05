#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use File::Basename;
use POSIX;

(my $scriptName, my $scriptPath, my $scriptSuffix) = fileparse( abs_path( $0 ) );

my $omniDir = $scriptPath;
$omniDir =~ s/\/scripts\///;

my $omniExec = $omniDir . "/omni/bin/omni ";
my $scriptFolder = $omniDir . "/scripts/BuildScripts";
my $cmdGeneric = $omniExec." --headless=".$scriptFolder;

sub buildRabbit {
    my $cmd = $cmdGeneric."/buildRabbit.omni.cmd";
    printTitle("rabbit", $cmd);
    system $cmd;
}

sub build150 {
    my $cmd = $cmdGeneric."/build150.omni.cmd";
    printTitle("150^3", $cmd);
    system $cmd;
}

sub build512 {
    my $cmd = $cmdGeneric."/build512.omni.cmd";
    printTitle("512^3", $cmd );
    system $cmd;
}

sub buildAll {
    buildRabbit();
    build150();
    build512();
}

sub printTitle {
    my $title = $_[0];
    my $cmd = $_[1];
    print "Running...(".$cmd.")\n";
    printLine();
    print "Building ".$title.":\n";
}

sub printLine {
    print "\n**********************************************\n";
}

sub menu {
    print "Build volumes menu:\n";
    print "0 -- build all\n";
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
		runMenuEntry( $answer );
		exit();
	    }
	}
    }
}

sub runMenuEntry {
    my $entry = $_[0];

    if( 0 == $entry ){
	buildAll();
    }elsif( 1 == $entry ){
	buildRabbit();
    }elsif( 2 == $entry ){
	build150();
    }elsif( 3 == $entry ){
	build512();
    }
}

sub checkCmdLineArgs {
    if ( 1 == @ARGV ) {
	runMenuEntry( $ARGV[0] );
    } else {
	menu();
    }
}

checkCmdLineArgs();
