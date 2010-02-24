#!/usr/bin/perl -w

use strict;

use Thread;

my $flag = "";
if (-t STDIN) {
}


sub runNode 
{
    my $node = $_[0];
    my $uptime = `ssh ${node} uptime | cut -f15 -d\\ `;
    print "$node\n";
    exit(0);
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
