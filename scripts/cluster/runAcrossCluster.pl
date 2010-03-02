#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use File::Basename;
use Thread;

my $cmd = $ARGV[0];
my $cmdIsFile = 0;
if (-e $cmd) {
    $cmdIsFile = 1;
    $cmd = abs_path( $cmd );
    if( $cmd =~ m/\.pl$/ ){
	$cmd = "perl " . $cmd;
    }
    $cmd = "$cmd &> /dev/null";
}

sub runNode 
{
    my $node = $_[0];

    if( $cmdIsFile ) {
	my $start = time();
	print "node $node: running command...\n"; 
	`ssh $node $cmd`; 
	my $end = time();
	my $timeSecs = ($end - $start);
	print "node $node: done (".$timeSecs." seconds)\n";
    } else {
	my $result = `ssh $node $cmd`;
	chomp($result);
	print "node $node: cmd output was \"$result\"\n";
    }
}

(my $name, my $path, my $suffix) = fileparse( abs_path( $0 ) );
open IN_FILE,  "<", "$path/hosts"  or die "could not read hosts";

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
