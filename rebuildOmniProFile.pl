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
my $includePath = $basePath.'/include';
my $testPath    = $basePath.'/tests';
my $omniProFile = $basePath.'/omni.common.pro';

my $strToPartiallyMatch = "of section to be rewritten using Perl";
my $delim = " \\\n\t";

my @cppFiles;
my @genFiles;
my @hppFiles;

sub processFile{
    my $file = $File::Find::name;

    # exclude files containg #
    if(/.*\#.*/){
	return;
    }

    if($file =~ m/.*thrift.*\.[hH](pp)?$/){
    processGen($file);
    }

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

    # skip skeleton files from thrift
    if($file =~ m/.*\.skeleton\.cpp$/){
    return;
    }

    push(@cppFiles, $file);
}

sub processHeader {
    my $file = $_[0];
    push(@hppFiles, $file);
}

sub processGen {
    my $file = $_[0];
    push(@genFiles, $file);
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

sub outputGenTargets {
    my(@files) = @_;

    @files = sort(@files);
    my @ret;
    foreach(@files){
        my $file = $_;

        if($file =~ m/\.skeleton\.cpp/) {
            next;
        }

        # escape special chars using \Q and \E
        $file =~ s/\Q$basePath\E\///g;
        my $target = $file;
        $target =~ s/^.*\///g;
        $target =~ s/\./_/g;
        my $ifFile = $file;
        $ifFile =~ s/^.*\///g;
        $ifFile =~ s/[_|\.].*/\.thrift/;

        push(@ret, "$target.target = $file\n");
        push(@ret, "$target.commands = external/libs/thrift/bin/thrift -r --out src/thrift --gen cpp if/$ifFile\n");
        push(@ret, "$target.depends = if/$ifFile\n");
        push(@ret, "QMAKE_EXTRA_TARGETS += $target\n\n");
    }

    return join("", @ret);
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
    $str .= "\n\n";
#    $str .= outputGenTargets(@genFiles);
#    $str .= "\n";
    $str .= "## end ".$strToPartiallyMatch."\n";
    return $str;
}

sub main{
    find(\&processFile, $srcPath);
    find(\&processFile, $libPath);
    rewriteOmniPro();
}

main();
