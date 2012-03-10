#!/usr/bin/perl

use strict;

my $basePath = `pwd`;
chomp $basePath;
my $omniPath    = $basePath.'/omni';
my $doxygenPath = $basePath.'/omni/docs/doxygen';

sub updateDoxygenDocs {
    chdir( $doxygenPath );
    `doxygen`;
    chdir( $basePath );
}

sub updateEtags {
    chdir( $omniPath );
    `find src/ -regex ".*\.[cChH]\(pp\)?" -print | etags -`;
    chdir( $basePath );
}

sub main {
    updateDoxygenDocs();
    updateEtags();
}

main();
