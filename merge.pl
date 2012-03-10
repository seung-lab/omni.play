#!/usr/bin/perl

# Purcaro March 2012
# requires https://github.com/apenwarr/git-subtree to be installed

use strict; 
use warnings;
use File::Path qw( rmtree );

my $tmp_folder = "/tmp/omni";

sub init {
    rmtree($tmp_folder);
    run("mkdir $tmp_folder");
    chdir $tmp_folder;
    run("git init");
    run("touch .gitignore");
    run("git add .gitignore");
    run("git commit -m'initial commit'");
}

sub run {
    my $cmd = $_[0];
    print "running \"$cmd\"\n";
    system($cmd);
}

sub gittree {
    my $name = $_[0];
    my $uri = $_[1];

    my $path = 'git@github.com:seung-lab/'.$uri;
    run("git subtree add --prefix=$name $path master");
}

sub nukeTarballs{
    run("git filter-branch --tree-filter 'rm -rf server/external/' HEAD");
    run("git filter-branch -f --tree-filter 'rm -rf common/external/' HEAD");
    run("git filter-branch -f --tree-filter 'rm -rf server/test/gtest-1.6.0/' HEAD");
    run("git gc --aggressive --prune");
}

init;

gittree("server", "omni.server.git");
gittree("common", "omni.common.git");
gittree("desktop", "omni.desktop.git");
gittree("filesystem","filesystem.git");
nukeTarballs
