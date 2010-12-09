#!/usr/bin/perl -w

# rewrite HEADERS and SOURCES sections of omni.pro
# will walk through src, tests, and libs folders

use strict;

use File::Path;
use File::Find;
use Tie::File;

my $basePath = `pwd`;
chomp $basePath;
my $srcPath     = $basePath.'/src';
my $libPath     = $basePath.'/lib';
my $testPath    = $basePath.'/tests';
my $omniProFile = $basePath.'/omni.pro';

my $strToPartiallyMatch = "of section to be rewritten using Perl";
my $delim = " \\\n\t";

my @cppFiles;
my @hppFiles;

sub processFile{
    my $file = $File::Find::name;
    if(/.*\.[cC](pp)?$/){
	processCPP($file);
    }elsif(/.*\.[hH](pp)?$/){
	processHeader($file);
    }
}

sub processCPP {
    my $file = $_[0];

    my $matlabOnlyFile = "watershed/QuickieWS.cpp";
    if($file =~ m/\Q$matlabOnlyFile\E/){
	return;
    }

    push(@cppFiles, $file);
}

sub processHeader {
    my $file = $_[0];
    push(@hppFiles, $file);
}

sub outputFileName {
    my(@files) = @_;

    @files = sort(@files);
    my @ret;
    foreach(@files){
	my $file = $_;
	# escape special chars using \Q and \E
	$file =~ s/\Q$basePath\E\///g;
	push(@ret, $file);
    }

    return join($delim, @ret);
}

sub rewriteOmniPro {
    my @lines;
    tie @lines, 'Tie::File', $omniProFile or die "could not open $omniProFile";

    my $firstLine = -1;
    my $lastLine  = -1;
    for(my $i = 0; $i < scalar(@lines); ++$i){
	if($lines[$i] =~ /\Q$strToPartiallyMatch\E/){
	    if(-1 == $firstLine){
		$firstLine = $i;
	    }elsif(-1 == $lastLine){
		$lastLine = $i;
	    }else{
		die ">2 strings found";
	    }
	}
    }

    if(-1 == $lastLine){
	die "did not find terminating line";
    }

    my $outStr = makeReplacementStr();
    splice(@lines, $firstLine, $lastLine-$firstLine+1, $outStr);

    untie @lines
}

sub makeReplacementStr {
    my $str = "";
    $str .= "## start ".$strToPartiallyMatch."\n";
    $str .= "HEADERS += $delim";
    $str .= outputFileName(@hppFiles);
    $str .= "\n\n";
    $str .= "SOURCES += $delim";
    $str .= outputFileName(@cppFiles);
    $str .= "\n";
    $str .= "## end ".$strToPartiallyMatch."\n";
    return $str;
}

sub main{
    find(\&processFile, $srcPath);
    find(\&processFile, $libPath);
    find(\&processFile, $testPath);
    rewriteOmniPro();
}

main();
