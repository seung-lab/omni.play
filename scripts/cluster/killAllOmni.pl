#!/usr/bin/perl -w

use strict;

use Thread;

my $flag = "";
if (-t STDIN) {
    print "press enter to killall omni, or 9 to force release of all locks\n";
    my $answer = <STDIN>;
    chomp ($answer);
    if ($answer eq "9") {
        print "press enter to killall -9 omni, this is your final warning, corruption of the omni file is likely.\n";
        $flag = "-9";
        $answer = <STDIN> if ($answer eq "9");
    }
}


sub runNode 
{
    my $node = $_[0];
    `ssh ${node} killall $flag omni`;
    `ssh ${node} ps -elf | grep omni`;
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
