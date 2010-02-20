#!/usr/bin/perl -w

use strict;
use Thread;

my $cmd = $ARGV[0];

sub runNode 
{
    my $node = $_[0];
    print "about to do".$cmd;
}

open IN_FILE,  "<", "hosts"  or die "could not read hosts";

my @threads;
while (my $line = <IN_FILE>) {
    chomp( $line );
    my $thr = new Thread \&runNode, $line;
    push(@threads, $thr);
}
close IN_FILE;

foreach my $thread (@threads){ 
    $thread->join;
}
