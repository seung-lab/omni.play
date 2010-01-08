#!/usr/bin/perl

$basepath = `pwd`;
chomp $basepath;

# Work around for a bug in the VTK tar ball.
`chmod 600 external/VTK/Utilities/vtktiff/tif_fax3sm.c`;

`echo "using mpi : /usr/bin/mpiCC ;" >> external/boost_1_38_0/tools/build/v2/user-config.jam`;
`echo "using mpi : /usr/bin/mpiCC ;" >> external/boost_1_38_0/user-config.jam`;

`echo "CMAKE_INSTALL_PREFIX:PATH=$basepath/external/libs/VTK/" >> $basepath/external/VTK/CMakeCache.txt`;
print "manually run: (cd $basepath/external/VTK; ccmake . && make -j5 && make install)\n";
print "(make sure to set debug flags!\n";
