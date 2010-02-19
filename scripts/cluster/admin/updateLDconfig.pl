#!/usr/bin/perl -w

use strict;

use Thread;

sub runNode 
{
    my $node = $_[0];
    my $start = time();
    print "node $node: ldconfig...\n"; 
    `ssh $node  /sbin/ldconfig`;
    my $end = time();
    my $timeSecs = ($end - $start);
    print "node $node: done ldconfig (".$timeSecs." seconds)\n";
}
open IN_FILE,  "<", "hosts"  or die "could not read hosts";

my @threads;
while (my $line = <IN_FILE>) {
    chomp( $line );
    my $thr = new Thread \&runNode, $line;
    push(@threads, $thr);
}
close IN_FILE;

my $count=0;
foreach my $thread (@threads){ 
    $thread->join;
    $count = $count +1;
}
