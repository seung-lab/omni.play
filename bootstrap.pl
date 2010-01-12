#!/usr/bin/perl

# NOTE: build process will be done "out-of-source". 
#  (from vtk): "When your build generates files, they have to go somewhere. 
#   An in-source build puts them in your source tree. 
#   An out-of-source build puts them in a completely separate directory, 
#   so that your source tree is unchanged."
# This allows us to untar only once, but build multiple times--much faster
#  on older computers!

use strict;

my $basePath = `pwd`;
chomp $basePath;
my $buildPath   = $basePath.'/external/builds';
my $srcPath     = $basePath.'/external/srcs';
my $libPath     = $basePath.'/external/libs';
my $tarballPath = $basePath.'/external/tarballs';
my $omniPath    = $basePath.'/omni';

my $globalMakeOptions = " -j5 ";

# Create build path if it doesn't exist yet.
print `mkdir $buildPath` if (!-e $buildPath);

# Create srcs path if it doesn't exist yet.
print `mkdir $srcPath` if (!-e $srcPath);

sub vtk {
    my $baseFileName = "vtk-5.4.2";
    prepare( $baseFileName );

    # Work around for a bug in the VTK tar ball.
    `chmod 600 $srcPath/$baseFileName/Utilities/vtktiff/tif_fax3sm.c`;
    
    `echo "CMAKE_INSTALL_PREFIX:PATH=$libPath/VTK/" >> $buildPath/$baseFileName/CMakeCache.txt`;

    print "manually run: (cd $buildPath/$baseFileName; ccmake $srcPath/$baseFileName && make -j5 && make install)\n";
    print "(make sure to set debug flags!)\n";
    print "\npress enter when vtk build is done :";
    $_ = <STDIN>;
}

sub setupBuildFolder {
    my $baseFileName = $_[0];

    print "==> creating new build folder...";
    `mkdir $buildPath/$baseFileName` if (!-e "$buildPath/$baseFileName" );
    print "done\n";
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

    if (-e "$srcPath/$baseFileName" ){
	print "==> skipping untar\n";
	return;
    }

    my $tarOptions = " -C $srcPath/ ";
    if( $baseFileName =~ /^vtk/ ) {
	`mkdir -p $srcPath/$baseFileName/`;
	$tarOptions = " -C $srcPath/$baseFileName/ --strip-components=1";
    }

    print "==> untarring to external/srcs/...";
    `tar -zxf $tarballPath/$baseFileName.tar.gz $tarOptions`;
    print "done\n";
}

sub prepare {
    my $baseFileName  = $_[0];
    my $libFolderName = $_[1];

    printTitle(        $baseFileName );
    nukeBuildFolder(   $baseFileName );
    nukeLibraryFolder( $libFolderName );
    untar(             $baseFileName );
    setupBuildFolder(  $baseFileName );
}

sub build {
    my $baseFileName  = $_[0];
    my $libFolderName = $_[1];
    my $buildOptions  = $_[2];

    chdir( "$buildPath/$baseFileName" );

    # TODO: check return values; die if something went wrong...
    configure(   $baseFileName, $libFolderName, $buildOptions );
    make();
    makeInstall();

    chdir( $basePath );
}

sub buildInSourceFolder {
    my $baseFileName  = $_[0];
    my $libFolderName = $_[1];
    my $buildOptions  = $_[2];

    chdir( "$srcPath/$baseFileName" );

    # TODO: check return values; die if something went wrong...
    configure(   $baseFileName, $libFolderName, $buildOptions );
    make();
    makeInstall();

    chdir( $basePath );
}

sub configure {
    my $baseFileName  = $_[0];
    my $libFolderName = $_[1];
    my $buildOptions  = $_[2];
    
    my $cmd = "$srcPath/$baseFileName/configure --prefix=$libPath/$libFolderName $buildOptions;";  
    if( "Qt" eq $libFolderName ){
	$cmd = 'echo "yes" | '.$cmd;
    }
    print "==> running configure...";
    print "($cmd)\n";
    print `($cmd)`;
    print "done with configure\n\n";
}

sub make {
    my $cmd = "make $globalMakeOptions";
    print "==> running make...";
    print "($cmd)\n";
    print `($cmd)`;
    print "done with make\n\n";
}

sub makeInstall {
    my $cmd = "make install";
    print "==> running make install...";
    print "($cmd)\n";
    print `($cmd)`;
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

    `echo "using mpi : /usr/bin/mpiCC ;" >> $srcPath/$baseFileName/tools/build/v2/user-config.jam`;
    `echo "using mpi : /usr/bin/mpiCC ;" >> $srcPath/$baseFileName/user-config.jam`;

    # as of Dec 2009, these are the portions of boost we appear to be using:
    # headers:   timer, shared_ptr, tuple, algorithm
    # libraries: filesystem, mpi, regex, serialization
    # to see all the boost libraries that can be built, do
    # ..../boost_1_38_0/configure --show-libraries
    
    # boost uses its own build folder; just symlink it to ours for consistency...
    my $boostLocalBuildFolder = "$srcPath/$baseFileName/bin.v2";
    `rm -rf $boostLocalBuildFolder`;
    `ln -s $buildPath/$baseFileName $boostLocalBuildFolder`;

    buildInSourceFolder( $baseFileName, "Boost", "--with-libraries=filesystem,mpi,regex,serialization" );
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
    # '-make libs' option only builds libs, shrinking installed size by 2 GB
    prepareAndBuild( $baseFileName, "Qt", "-opensource -static -no-glib -fast -make libs -no-accessibility -no-qt3support -no-cups -no-qdbus " );
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
    print "3 -- Build qt\n";
    print "4 -- Build vtk\n";
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
	qt();
    }elsif( 4 == $entry ){
	vtk();
    }elsif( 5 == $entry ){
	omni();
    }elsif( 6 == $entry ){
	smallLibraries();
	boost();
	qt();
	vtk();
	omni();
    }
}

menu();
