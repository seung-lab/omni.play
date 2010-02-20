#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use Thread;

my $cmd = abs_path( $ARGV[0] );

if( $cmd =~ m/\.pl$/ ){
    $cmd = "perl " . $cmd;
}

sub runNode 
{
    my $node = $_[0];
    my $start = time();
    print "node $node: running command...\n"; 
    `ssh $node $cmd &> /dev/null`; 
    my $end = time();
    my $timeSecs = ($end - $start);
    print "node $node: done (".$timeSecs." seconds)\n";
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
