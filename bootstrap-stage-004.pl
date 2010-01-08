#!/usr/bin/perl

$basepath = `pwd`;
chomp $basepath;

printTitle("qt");
print "(cd $basepath/external/qt-all-opensource-src-4.5.2; ./configure --prefix=$basepath/external/libs/Qt -opensource -static -no-glib; make -j5 && make install)\n";
print `(cd $basepath/external/qt-all-opensource-src-4.5.2; echo "yes" | ./configure --prefix=$basepath/external/libs/Qt -opensource -static -no-glib; make -j5 && make install)`;


sub printTitle
{
    my $title = $_[0];
    printLine();
    print $title.":\n";
}

sub printLine
{
    print "**********************************************\n";
}
