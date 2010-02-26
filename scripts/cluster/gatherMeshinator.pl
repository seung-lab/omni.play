#!/usr/bin/perl -w

use strict;

use Thread;

sub runNode 
{
    my $node = $_[0];
    `mkdir -p /home/mwimer/${node}/`;
    print "`ssh ${node} cp /tmp/meshinator* /home/mwimer/${node}/`";
    `ssh ${node} cp /tmp/meshinator* /home/mwimer/${node}/`;
}

open IN_FILE,  "<", "hosts"  or die "could not read hosts";

my @threads;
while (my $line = <IN_FILE>) {
    chomp( $line );
    my $thr = new Thread \&runNode, $line;
    $thr->join();
    push(@threads, $thr);
}
close IN_FILE;
