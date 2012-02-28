#!/usr/bin/perl -w

use strict;
use Cwd 'abs_path';
use File::Basename;
use POSIX;

my ($name, $path, $suffix) = fileparse(abs_path($0));

chdir($path);

my $svn_ver = `svnversion ../../../`;
$svn_ver =~ s/(.*)\://;
$svn_ver =~ s/M//;

`perl -i -pe 's/Revision\:(.*)/Revision\:$svn_ver/' Doxyfile`;
`doxygen`;
