#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use File::Basename;
use POSIX;
use Thread;
use List::Util qw(shuffle);

select STDOUT; $| = 1;
#$SIG{ALRM} = \&exitUptime;
alarm 2;

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
    my $uptime = `rsh ${node} uptime`;
    my @upsplit = split (/ /, $uptime);
    $uptime = $upsplit[scalar(@upsplit) - 2];
    print "$uptime $node\n" if ("" ne $uptime);
}

open IN_FILE,  "<", "$hosts"  or die "could not read hosts";

my @threads;
my @lines;
while (my $line = <IN_FILE>) {
    chomp( $line );
    push(@lines, $line);
}
@lines = shuffle @lines;
while (my $line = pop @lines) {
    my $thr = new Thread \&runNode, $line;
    push(@threads, $thr);
}
close IN_FILE;

my $count=0;
foreach my $thread (@threads){ 
    $thread->join;
    $count = $count +1;
}
