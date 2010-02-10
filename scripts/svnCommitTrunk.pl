#!/usr/bin/perl -w

use strict;

my $staging_folder = "$ENV{HOME}/.omni.Staging.shadow";

if( !-d $staging_folder ) {
   # if svnInfo contains svn_ssh, complain/die to run 
   #    svn co file:///gaba/svn/netgpu/omni/staging/trunk ~/.omni.Stating.shadow
    print "failure! get your shadow .omni folder this way:";
    print  "svn co file:///gaba/svn/netgpu/omni/staging/trunk ~/.omni.Staging.shadow\n";
    die;
}

#test `svn info` to make sure it is trunk; if not, complain/exit

my $diff_add = `svn diff | grep '^+++'`;
my $diff_sub = `svn diff | grep '^\-\-\-'`;

print "changes:\n";
print $diff_add;
print $diff_sub;

print "\nplease enter commit message (don't screw up!):  ";
my $commit_msg = <STDIN>;

$commit_msg = $commit_msg."\n".$diff_add."\n".$diff_sub;

print "intended commit msg:\n".$commit_msg;
print "is this ok? control-c to cancel; enter to continue";
my $ans = <STDIN>;

my $tmpLogFileName = $staging_folder."/tmpLogMsg.txt";
open OUT_FILE, ">", $tmpLogFileName or die "could not read $tmpLogFileName";
print OUT_FILE $commit_msg;
close OUT_FILE;

print "updating src in current folder...";
`svn up`; 
print "done\n";

`svn commit -F $tmpLogFileName`;
`cd ~/.omni.Staging.shadow`; #cd to .omni.Stating.shadow
`svnmerge.py merge`;
`svn commit -m $commit_msg`;
