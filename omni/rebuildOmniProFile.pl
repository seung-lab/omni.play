#!/usr/bin/perl -w

# NOTE: build process will be done "out-of-source".
#  (from vtk): "When your build generates files, they have to go somewhere.
#   An in-source build puts them in your source tree.
#   An out-of-source build puts them in a completely separate directory,
#   so that your source tree is unchanged."
# This allows us to untar only once, but build multiple times--much faster
#  on older computers!

use strict;

use File::Path;
use File::Find;

my $basePath = `pwd`;
chomp $basePath;
my $srcPath     = $basePath.'/src';
my $omniProFile = $basePath.'/omni.pro';

my @cppFiles;
my @hppFiles;

sub processFile{
    my $file = $File::Find::name;
    if(/.*\.[cC](pp)?$/){
	processCPP($file);
    }
    if(/.*\.[hH](pp)?$/){
	processHeader($file);
    }
}

sub processCPP {
    my $file = $_[0];
    push(@cppFiles, $file);
}

sub processHeader {
    my $file = $_[0];
    push(@hppFiles, $file);
}

sub outputFileName {
    my(@files) = @_;

    @files = sort(@files);

    my $p =
    foreach(@files){
	my $file = $_;
	$file = "\$file = ~s/$basePath//g";
	print $file."\n";
    }
}

sub main{
    find(\&processFile, $srcPath);
    outputFileName(@cppFiles);
    outputFileName(@hppFiles);
}

main();
