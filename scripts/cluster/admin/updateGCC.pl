#!/usr/bin/perl -w

use strict;

use Thread;

sub runNode 
{
    my $node = $_[0];
#    gmp( $node );
#    mpfr( $node );
    gcc( $node );
}

sub gmp {
    my $node = $_[0];
    my $start = time();
    print "node $node: installing gmp...\n"; 
    `ssh $node  perl /home/purcaro/updates/gcc/updateGMP.pl &> /dev/null`; 
    my $end = time();
    my $timeSecs = ($end - $start);
    print "node $node: done installing gmp (".$timeSecs." seconds)\n";
}

sub gcc {
    my $node = $_[0];
    print "node $node: installing gcc...\n"; 
    my $start = time();
    `ssh $node  perl /home/purcaro/updates/gcc/installGCC-4.3.4.pl &> /dev/null`;
    my $end = time();
    my $timeSecs = ($end - $start);
    print "node $node: done installing gcc (".$timeSecs." seconds)\n";
}

sub mpfr  {
    my $node = $_[0];
    print "node $node: installing mpfr...\n"; 
    my $start = time();
    `ssh $node  perl /home/purcaro/updates/gcc/updateMPFR.pl &> /dev/null`;
    my $end = time();
    my $timeSecs = ($end - $start);
    print "node $node: done installing mpfr (".$timeSecs." seconds)\n";
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
