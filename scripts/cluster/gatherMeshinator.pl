#!/usr/bin/perl -w

use strict;

use Thread;
my $output = $ENV{HOME} if (!defined $ARGV[0]);
$output = $ARGV[0] if (!defined $output);

sub runNode 
{
    my $node = $_[0];
    `mkdir -p $output/${node}/`;
    print "gathering with: `ssh ${node} cp /tmp/meshinator\* $output/${node}/`\n";
    `ssh ${node} cp /tmp/meshinator\* $output/${node}/`;
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
