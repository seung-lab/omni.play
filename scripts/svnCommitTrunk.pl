#!/usr/bin/perl -w

use strict;

my $src="TRUNK---------->";
my $dest="STAGING (shadow)---------->";
my $staging_folder = "$ENV{HOME}/.omni.staging.shadow/";
my $prefixMessgeForShadowSVNcmmit = "automerge from trunk: ";
my $tmpLogFileName = $staging_folder."tmpLogMsg.txt";

sub writeTempFile{
    my $str = $_[0];
    open OUT_FILE, ">", $tmpLogFileName or die "could not open $tmpLogFileName";
    print OUT_FILE $str;
    close OUT_FILE;
}

if( !-d $staging_folder ) {
    print "failure! get your shadow .omni folder this way:\n";
    print  "\tsvn co file:///gaba/svn/netgpu/omni/staging/trunk $staging_folder\n";
    die;
}

#test `svn info` to make sure it is trunk; if not, complain/exit

my $diff = `svn status | grep -v '\?'`;

print "$src changes:\n";
print $diff;

print "$src please enter commit message: ";
my $commit_msg = <STDIN>;

$commit_msg = $commit_msg.$diff;

print "$src intended commit msg:\n".$commit_msg;
print "$src is this ok? (control-c to cancel; enter to continue): ";
my $ans = <STDIN>;

writeTempFile( $commit_msg );

print "$src updating src in current folder...\n";
print `svn up`; 
print "done\n";

print "$src committing to repository...\n";
print `svn commit -F $tmpLogFileName`;
print "done\n";

print "$dest reverting any local changes...\n";
print `svn revert $staging_folder`;
print `svn revert -R $staging_folder/*`;
print "done\n";

print "$dest updating...\n";
print `svn up $staging_folder`;
print "done\n";

print "$dest merging from trunk...\n";
print `$staging_folder/external/svnmerge.py merge $staging_folder`;
print "done\n";

writeTempFile( $prefixMessgeForShadowSVNcmmit.$commit_msg );

print "$dest merging into staging complete; committing...\n";
print `svn commit -F $tmpLogFileName $staging_folder`;
print "done\n";

`rm $tmpLogFileName`;

print "****************************\n";
print "All Done\n";
