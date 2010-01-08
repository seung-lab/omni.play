#!/usr/bin/perl

$base = `pwd`;
chomp $base;

sub nukeFolders
{
    print "Cleaning old libraries from $base (ignore not founds errors)...\n";
    `rm -rf $base/external/boost_1_38_0`;
    `rm -rf $base/external/expat-2.0.1`;
    `rm -rf $base/external/fontconfig-2.7.1`;
    `rm -rf $base/external/freetype-2.3.9`;
    `rm -rf $base/external/freetype-2.3.9`;
    `rm -rf $base/external/hdf5-1.6.9`;
    `rm -rf $base/external/libpng-1.2.39`;
    `rm -rf $base/external/qt-all-opensource-src-4.5.2`;
    `rm -rf $base/external/VTK`;
}

sub doUntar
{
    print "Untarring libraries...\n";
    `(cd $base/external/; tar -zxvf boost_1_38_0.tar.gz)`;
    `(cd $base/external/; tar -zxvf freetype-2.3.9.tar.gz)`;
    `(cd $base/external/; tar -zxvf qt-all-opensource-src-4.5.2.tar.gz)`;
    `(cd $base/external/; tar -zxvf expat-2.0.1.tar.gz)`;
    `(cd $base/external/; tar -zxvf hdf5-1.6.9.tar.gz)`;
    `(cd $base/external/; tar -zxvf vtk-5.4.2.tar.gz)`;
    `(cd $base/external/; tar -zxvf fontconfig-2.7.1.tar.gz)`;
    `(cd $base/external/; tar -zxvf libpng-1.2.39.tar.gz)`;

}

nukeFolders();
doUntar();
