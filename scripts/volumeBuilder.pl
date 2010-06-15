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

sub build150noMeshes {
    my $cmd = $cmdGeneric."/build150.noMeshes.omni.cmd";
    printTitle("150^3 no meshes", $cmd);
    system $cmd;
}

sub build512 {
    my $cmd = $cmdGeneric."/build512.omni.cmd";
    printTitle("512^3", $cmd );
    system $cmd;
}

sub build512noMeshes {
    my $cmd = $cmdGeneric."/build512.noMeshes.omni.cmd";
    printTitle("512^3 no meshes", $cmd );
    system $cmd;
}

sub build512useMeshinator {
    my $cmd = $cmdGeneric."/build512useMeshinator.omni.cmd";
    printTitle("512^3 use Meshinator", $cmd );
    system $cmd;
}

###########################################

sub buildRegionGraphNew150 {
    my $cmd = $cmdGeneric."/regionGraphs/new150graph.omni.cmd";
    printTitle("new 150 w/ region graph", $cmd);
    system $cmd;
}

sub buildRegionI1088150 {
    my $cmd = $cmdGeneric."/regionGraphs/build.i1088_150_graph.omni.cmd";
    printTitle("i1088 150 w/ region graph", $cmd);
    system $cmd;
}

sub buildRegionI1088250 {
    my $cmd = $cmdGeneric."/regionGraphs/build.i1088_250_graph.omni.cmd";
    printTitle("i1088 250 w/ region graph", $cmd);
    system $cmd;
}

sub buildAll {
    buildRabbit();
    build150();
    build512();
}

sub buildAllRegion {
    buildRegionGraphNew150();
    buildRegionI1088150();
    buildRegionI1088250();
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
    print "4 -- Build 150^3 (no meshes)\n";
    print "5 -- Build 512^3 (no meshes)\n";
    print "6 -- Build 512^3 (use meshinator)\n";
    print "7 -- Build all region graphs\n";
    print "8 -- Build new150graph w/ region graphs\n";
    print "9 -- Build i1088_150 w/ region graphs\n";
    print "10 -- Build i1088_250 w/ region graphs\n";
    print "\n";
    my $max_answer = 10;

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
    }elsif( 4 == $entry ){
	build150noMeshes();
    }elsif( 5 == $entry ){
	build512noMeshes();
    }elsif( 6 == $entry ){
	build512useMeshinator();
    }elsif( 7 == $entry ){
	buildAllRegion();
    }elsif( 8 == $entry ){
	buildRegionGraphNew150();
    }elsif( 9 == $entry ){
	buildRegionI1088150();	
    }elsif( 10 == $entry ){
	buildRegionI1088250();	
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
