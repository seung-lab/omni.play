#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use File::Basename;
use POSIX;
use Thread;

select STDOUT; $| = 1;
#$SIG{ALRM} = \&exitUptime;
alarm 1;

sub exitUptime
{
  print "exiting...";
  exit(0);
}

(my $name, my $path, my $suffix) = fileparse( abs_path( $0 ) );
my $home = $path;
my $hosts = "$home/hosts";

sub runNode 
{
    my $node = $_[0];
    my $uptime = `ssh ${node} uptime | cut -f15 -d\\ `;
    chop($uptime); chop($uptime);
    print "$uptime $node\n";
}

open IN_FILE,  "<", "$hosts"  or die "could not read hosts";

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
