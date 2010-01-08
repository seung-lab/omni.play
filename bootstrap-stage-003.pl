#!/usr/bin/perl

$basepath = `pwd`;
chomp $basepath;

printTitle("Boost");
print `(cd $basepath/external/boost_1_38_0; ./configure --prefix=$basepath/external/libs/Boost;          make -j5 && make install)`;

printTitle("libpng");
print `(cd $basepath/external/libpng-1.2.39; ./configure --prefix=$basepath/external/libs/libpng;        make -j5 && make install)`;

printTitle("FreeType");
print `(cd $basepath/external/freetype-2.3.9; ./configure --prefix=$basepath/external/libs/FreeType;     make -j5 && make install)`;

printTitle("fontconfig");
print `(cd $basepath/external/fontconfig-2.7.1; ./configure --prefix=$basepath/external/libs/Fontconfig; make -j5 && make install)`;

printTitle("expat");
print `(cd $basepath/external/expat-2.0.1; ./configure --prefix=$basepath/external/libs/expat;           make -j5 && make install)`;

printTitle("hdf5");
print `(cd $basepath/external/hdf5-1.6.9; ./configure --prefix=$basepath/external/libs/HDF5 --enable-threadsafe --with-pthread=/usr/lib --enable-shared=no; make -j5 && make install)`;

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
