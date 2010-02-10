#!/usr/bin/perl -w

use strict;

my $src="TRUNK---------->";
my $dest="STAGING---------->";

my $staging_folder = "$ENV{HOME}/.omni.Staging.shadow";

if( !-d $staging_folder ) {
   # if svnInfo contains svn_ssh, complain/die to run 
   #    svn co file:///gaba/svn/netgpu/omni/staging/trunk ~/.omni.Stating.shadow
    print "failure! get your shadow .omni folder this way:\n";
    print  "svn co file:///gaba/svn/netgpu/omni/staging/trunk ~/.omni.Staging.shadow\n";
    die;
}

#test `svn info` to make sure it is trunk; if not, complain/exit

my $diff_add = `svn diff | grep '^+++'`;
my $diff_sub = `svn diff | grep '^\-\-\-'`;

print "$src changes:\n";
print $diff_add;
print $diff_sub;

print "$src please enter commit message (don't screw up!): ";
my $commit_msg = <STDIN>;

$commit_msg = $commit_msg.$diff_add.$diff_sub;

print "$src intended commit msg:\n".$commit_msg;
print "$src is this ok? (control-c to cancel; enter to continue): ";
my $ans = <STDIN>;

my $tmpLogFileName = $staging_folder."/tmpLogMsg.txt";
open OUT_FILE, ">", $tmpLogFileName or die "could not open $tmpLogFileName";
print OUT_FILE $commit_msg;
close OUT_FILE;

print "$src updating src in current folder...\n";
print `svn up`; 
print "done\n";

print "$src committing to repository...\n";
print `svn commit -F $tmpLogFileName`;
print "done\n";

print "$dest updating...\n";
print `svn up $staging_folder`;
print "done\n";

print "$dest merging from trunk...\n";
print `$staging_folder/external/svnmerge.py merge $staging_folder`;
print "done\n";

print "$dest merging into staging complete; commit the changes? (enter to continue): ";
$ans = <STDIN>;
print "OK\n";

print "$dest: committing...\n";
print `svn commit -F $tmpLogFileName $staging_folder/`;
print "done\n";

`rm $tmpLogFileName`;

print "\nAll Done\n";

