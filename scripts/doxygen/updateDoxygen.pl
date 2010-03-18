#!/usr/bin/perl -w

use strict;
use XML::Simple;
use Cwd 'abs_path';
use File::Basename;
use File::Copy;
use POSIX;
use Time::HiRes;

my $minutesBetweenUpdates = 6*60;

my ($script_name, $script_path, $script_suffix) = fileparse( abs_path( $0 ) );

my $root = "/Users/purcaro/gliamacDoxygen";
my $main_output = $root."/www/doxygenOutput";
my @folders = ("omni.src.dev",
	       "omni.src.stable",
	       "omni.src.staging",
	       "omni.src.trunk" );

sub main {
    while(1){
	runFolders();
	updateIndex();
	sleep 60 * $minutesBetweenUpdates;
    }
}

sub updateIndex {
    my $in_file_path  = $script_path."/index.html";
    my $out_file_path = $main_output."/index.html";
    copy( $in_file_path, $out_file_path ) or die "Could not copy index.html file";
}

sub runFolders {
    foreach (@folders) {
	my $start = Time::HiRes::gettimeofday();

	updateFolder( $_ );

	my $end = Time::HiRes::gettimeofday();
	printf("(completed in %.2f seconds)\n", $end - $start);
    }
}

sub updateFolder {
	my $folder = $_;
	my $path = $root."/".$folder;   
	print $path."\n";

	print "==> updating src...";
	`svn up $path`;
	print "done\n";

	my $xml_log = `svn log -l 1 --with-no-revprops --xml $path`;
	my $xs1 = XML::Simple->new();
	my $doc = $xs1->XMLin($xml_log);
	my $svn_ver = $doc->{logentry}->{revision};
	print "\tat revision: ".$svn_ver."\n";

	my $output_folder = $main_output."/".$folder."/".$svn_ver;
	if( -d $output_folder ){
	    print "skipping...\n";
	    return;
	}

	`mkdir -p $output_folder`;

	updateDoxygenConfigFile( $svn_ver, $output_folder, $path );

	print "==> running doxygen...\n\tdoxygen warnings:\n";
	`doxygen`;
	print "\n\ndone!\n";
}

sub updateDoxygenConfigFile {
    my $svn_ver = $_[0];
    my $output_path = $_[1];    
    my $path = $_[2];

    my $inFileName  = "$script_path/Doxyfile.template";
    my $outFileName = "$script_path/Doxyfile";

    open IN_FILE,  "<", $inFileName  or die "could not read $inFileName";
    open OUT_FILE, ">", $outFileName or die "could not write $outFileName";;

    while (my $line = <IN_FILE>) { 
	if( $line =~ /^PROJECT_NUMBER / ) {
	    print OUT_FILE "PROJECT_NUMBER = Revision:".$svn_ver."\n";
	} elsif ( $line =~ /^OUTPUT_DIRECTORY / ) {
	    print OUT_FILE "OUTPUT_DIRECTORY = ".$output_path."\n";
	} elsif ( $line =~ /^INPUT  / ) {
	    print OUT_FILE "INPUT = ".$path."\n";
	} else {
	    print OUT_FILE $line;
	}
    }
    
    close OUT_FILE;
    close IN_FILE;
}

main();
