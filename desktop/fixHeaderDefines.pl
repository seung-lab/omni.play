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
my $testPath    = $basePath.'/tests';

my @hppFiles;

sub processFile{
    my $file = $File::Find::name;

    # exclude files containg #
    if(/.*\#.*/){
	return;
    }

    if(/.*\.[hH](pp)?$/){
	processHeader($file);
    }
}

sub processHeader {
    my $file = $_[0];
    push(@hppFiles, $file);
}

sub rewriteHeaders {

    foreach(@hppFiles)
    {
	my $file = $_;
        my @lines;
        tie @lines, 'Tie::File', $file or die "could not open $file";

        if(@lines && $lines[0] =~ m/^#ifndef/)
        {
            shift(@lines);
            $lines[0] = "#pragma once";

            while(1)
            {
                if(0 == scalar(@lines)){
                    die("no lines left in file $file");
                }

                my $line = pop(@lines);
                if($line)
                {
                    if($line =~ m/^#endif/){
                        last;
                    }
                }
            }
        }

        untie @lines
    }
}

sub main{
    find(\&processFile, $srcPath);
    find(\&processFile, $testPath);
    rewriteHeaders();
}

main();
