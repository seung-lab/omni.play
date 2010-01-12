#!/usr/bin/perl

use strict;

my $basePath = `pwd`;
chomp $basePath;
my $buildPath   = $basePath.'/external/builds';
my $libPath     = $basePath.'/external/libs';
my $tarballPath = $basePath.'/external/tarballs';
my $omniPath    = $basePath.'/omni';

# Create build path if it doesn't exist yet.
print `mkdir $buildPath` if (!-e $buildPath);

sub vtk {
    my $baseFileName = "vtk-5.4.2";
    printTitle( $baseFileName );
    nukeBuildFolder(   "VTK" );
    nukeLibraryFolder( "VTK" );
    untar(      $baseFileName );

    # Work around for a bug in the VTK tar ball.
    `chmod 600 $buildPath/VTK/Utilities/vtktiff/tif_fax3sm.c`;
    
    `echo "CMAKE_INSTALL_PREFIX:PATH=$libPath/VTK/" >> $buildPath/VTK/CMakeCache.txt`;

    print "manually run: (cd $buildPath/VTK; ccmake . && make -j5 && make install)\n";
    print "(make sure to set debug flags!)\n";
    print "\npress enter when vtk build is done :";
    $_ = <STDIN>;
}

sub nukeBuildFolder {
    my $baseFileName = $_[0];

    print "==> removing old build folder...";
    `rm -rf $buildPath/$baseFileName`;
    print "done\n";
}

sub nukeLibraryFolder {
    my $libFolderName = $_[0];

    print "==> removing old library folder...";
    `rm -rf $libPath/$libFolderName`;
    print "done\n";
}

sub untar {
    my $baseFileName = $_[0];

    print "==> untarring...";
    `tar -zxf $tarballPath/$baseFileName.tar.gz -C $buildPath/`;
    print "done\n";
}

sub prepare {
    my $baseFileName  = $_[0];
    my $libFolderName = $_[1];

    printTitle(        $baseFileName );
    nukeBuildFolder(   $baseFileName );
    nukeLibraryFolder( $libFolderName );
    untar(             $baseFileName );
}

sub build {
    my $baseFileName  = $_[0];
    my $libFolderName = $_[1];
    my $buildOptions  = $_[2];

    print "==> running configure...";
    print "(cd $buildPath/$baseFileName; ./configure --prefix=$libPath/$libFolderName $buildOptions; )\n";
    print `(cd $buildPath/$baseFileName; ./configure --prefix=$libPath/$libFolderName $buildOptions; )`;
    print "done with configure\n\n";

    # TODO: check return value; die if something went wrong...
    print "==> running make...";
    print "(cd $buildPath/$baseFileName; make -j5)\n";
    print `(cd $buildPath/$baseFileName; make -j5)`;
    print "done with make\n\n";

    print "==> running make install...";
    print "(cd $buildPath/$baseFileName; make install)\n";
    print `(cd $buildPath/$baseFileName; make install)`;
    print "done with make install\n";
}

sub prepareAndBuild {
    my $baseFileName  = $_[0];
    my $libFolderName = $_[1];

    my $buildOptions = "";
    if( scalar(@_) > 2 ) {
	$buildOptions = $_[2];
    }

    prepare( $baseFileName, $libFolderName );
    build(   $baseFileName, $libFolderName, $buildOptions );
}

sub boost {
    my $baseFileName = "boost_1_38_0";
    prepare( $baseFileName, "Boost" );

    `echo "using mpi : /usr/bin/mpiCC ;" >> $buildPath/$baseFileName/tools/build/v2/user-config.jam`;
    `echo "using mpi : /usr/bin/mpiCC ;" >> $buildPath/$baseFileName/user-config.jam`;

    # as of Dec 2009, these are the portions of boost we appear to be using:
    # headers:   timer, shared_ptr, tuple, algorithm
    # libraries: filesystem, mpi, regex, serialization
    # to see all the boost libraries that can be built, do
    # ..../boost_1_38_0/configure --show-libraries

    build( $baseFileName, "Boost", "--with-libraries=filesystem,mpi,regex,serialization" );
}

sub libpng {
    prepareAndBuild( "libpng-1.2.39", "libpng" );
}

sub freetype {
    prepareAndBuild( "freetype-2.3.9", "FreeType" );
}

sub fontconfig {
    prepareAndBuild( "fontconfig-2.7.1", "Fontconfig" );
}

sub expat {
    prepareAndBuild( "expat-2.0.1", "expat" );
}

sub hdf5 {
    prepareAndBuild( "hdf5-1.6.9", "HDF5", "--enable-threadsafe --with-pthread=/usr/lib --enable-shared=no" );
}

sub qt {
    my $baseFileName = "qt-all-opensource-src-4.5.2";
    prepare( $baseFileName, "Qt" );

    print "(cd $buildPath/$baseFileName; ./configure --prefix=$libPath/Qt -opensource -static -no-glib -make libs; make -j5 && make install)\n";
    print `(cd $buildPath/$baseFileName; echo "yes" | ./configure --prefix=$libPath/Qt -opensource -static -no-glib -make libs; make -j5 && make install)`;
}

sub omni {
    my $cmakeSettings = <<END;
CMAKE_BUILD_TYPE:STRING=Debug
CMAKE_INSTALL_PREFIX:STRING=$buildPath
QT_QMAKE_EXECUTABLE:STRING=$libPath/Qt/bin/qmake
DESIRED_QT_VERSION:STRING=4

Boost_INCLUDE_DIR:PATH=$libPath/Boost/include/boost-1_38/
Boost_LIBRARY_DIRS:FILEPATH=$libPath/Boost/
Boost_USE_MULTITHREADED:BOOL=ON
END

    `echo "$cmakeSettings" > $omniPath/CMakeCache.txt`;
    `rm -rf $omniPath/CMakeFiles`;
}

sub checkBashRC {
    # TODO!
}

sub printTitle {
    my $title = $_[0];
    printLine();
    print $title.":\n";
}

sub printLine {
    print "\n**********************************************\n";
}

sub smallLibraries {
    libpng();
    freetype();
    fontconfig();
    expat();
    hdf5();
}

sub menu {
    my $answer = -1;

    print "bootstrap.pl menu:\n";
    print "0 -- exit\n";
    print "1 -- Build small libs\n";
    print "2 -- Build boost\n";
    print "3 -- Build vtk\n";
    print "4 -- Build qt\n";
    print "5 -- Setup omni build\n";
    print "6 -- [Do 1 through 5]\n\n";
    
    while( 1 ){
	print "Please make selection: ";
	$answer = <STDIN>;

	if( $answer =~ /^\d$/ ) {
	    if( ($answer > -1) and ($answer < 7)){
		runMenuEntry( $answer );
		exit();
	    }
	}
    }
}

sub runMenuEntry {
    my $entry = $_[0];

    if( 0 == $entry ){
	exit();
    }elsif( 1 == $entry ){
	smallLibraries();
    }elsif( 2 == $entry ){
	boost();
    }elsif( 3 == $entry ){
	vtk();
    }elsif( 4 == $entry ){
	qt();
    }elsif( 5 == $entry ){
	omni();
    }elsif( 6 == $entry ){
	smallLibraries();
	boost();
	vtk();
	qt();
	omni();
    }
}

menu();
