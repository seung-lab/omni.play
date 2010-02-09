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
my $scriptPath  = $basePath.'/scripts';
my $omniPath    = $basePath.'/omni';

my $globalMakeOptions = " -j15 ";

# Create build path if it doesn't exist yet.
print `mkdir $buildPath` if (!-e $buildPath);

# Create srcs path if it doesn't exist yet.
print `mkdir $srcPath` if (!-e $srcPath);

sub isMac {
    my $uname = `uname`;
    return ($uname =~ /Darwin/);
}

sub genOmniScript {
    open (SCRIPT, ">", "$scriptPath/buildomni.sh") or die $!;

    my $script = <<END;
export QTDIR=$libPath/Qt
export BOOST_ROOT=$libPath/Boost
export EXPAT_INCLUDE=$libPath/expat/lib/
export EXPAT_LIBPATH=$libPath/expat/include/
cd $basePath/omni
cmake .
make $globalMakeOptions
END
    print SCRIPT $script;
    close SCRIPT;
    `chmod +x $scriptPath/buildomni.sh`;
}

sub genVTKscript {
    my $baseFileName = $_[0];
    open (SCRIPT, ">", "$scriptPath/buildvtk.sh") or die $!;

    my $script = <<END;
cd $buildPath/$baseFileName
cmake $srcPath/$baseFileName
make -k
make install    
END
    print SCRIPT $script;
    close SCRIPT;
    `chmod +x $scriptPath/buildvtk.sh`;
}

sub vtk {
    my $baseFileName = "vtk-5.4.2";
    genVTKscript( $baseFileName );
    prepare( $baseFileName, "VTK" );

    # Work around for a bug in the VTK tar ball.
    `chmod 600 $srcPath/$baseFileName/Utilities/vtktiff/tif_fax3sm.c`;
    
    `echo "CMAKE_INSTALL_PREFIX:PATH=$libPath/VTK/" >> $buildPath/$baseFileName/CMakeCache.txt`;
    if (isMac()){
	`echo "BUILD_SHARED_LIBS:BOOL=ON" >> $buildPath/$baseFileName/CMakeCache.txt` 
    }
    `echo "CMAKE_BUILD_TYPE:STRING=Debug" >> $buildPath/$baseFileName/CMakeCache.txt`;
    `echo "CMAKE_CXX_FLAGS_DEBUG:STRING=-g -I $libPath/libtiff/include" >> $buildPath/$baseFileName/CMakeCache.txt`;
    `echo "BUILD_TESTING:BOOL=OFF" >> $buildPath/$baseFileName/CMakeCache.txt`;

    #`patch $srcPath/$baseFileName/Utilities/MaterialLibrary/ProcessShader.cxx -i $basePath/external/patches/vtk-processshader.patch`;

    print "runing: (cd $buildPath/$baseFileName; ccmake $srcPath/$baseFileName && make && make install)\n";
    `sh $scriptPath/buildvtk.sh`;
}

sub setupBuildFolder {
    my $baseFileName = $_[0];

    print "==> creating new build folder...";
    `mkdir $buildPath/$baseFileName` if (!-e "$buildPath/$baseFileName" );
    print "done\n";
}

sub nukeSrcsFolder {
    my $baseFileName = $_[0];

    print "==> removing old srcs folder...";
    `rm -rf $srcPath/$baseFileName`;
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

sub prepareNukeSrcsFolder {
    my $baseFileName  = $_[0];
    my $libFolderName = $_[1];

    printTitle(        $baseFileName );
    nukeSrcsFolder(    $baseFileName );
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
    # TODO: check return values; die if something went wrong...
    print `($cmd)`;
    print "done with configure\n\n";
}

sub make {
    my $cmd = "make $globalMakeOptions";
    print "==> running make...";
    print "($cmd)\n";
    # TODO: check return values; die if something went wrong...
    print `($cmd)`;
    print "done with make\n\n";
}

sub makeInstall {
    my $cmd = "make install";
    print "==> running make install...";
    print "($cmd)\n";
    # TODO: check return values; die if something went wrong...
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
    # libraries: filesystem, mpi, regex, serialization, thread (for # of processors)
    # to see all the boost libraries that can be built, do
    # ..../boost_1_38_0/configure --show-libraries
    
    # boost uses its own build folder; just symlink it to ours for consistency...
    my $boostLocalBuildFolder = "$srcPath/$baseFileName/bin.v2";
    `rm -rf $boostLocalBuildFolder`;
    `ln -s $buildPath/$baseFileName $boostLocalBuildFolder`;

    buildInSourceFolder( $baseFileName, "Boost", "--with-libraries=filesystem,mpi,regex,serialization,thread" );
}

sub libpng {
    prepareAndBuild( "libpng-1.2.39", "libpng" );
}

sub libtiff { 
    prepareNukeSrcsFolder( "tiff-3.8.2", "libtiff" );
    buildInSourceFolder( "tiff-3.8.2", "libtiff", "--without-jpeg" );
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
    prepareAndBuild( "hdf5-1.6.9", "HDF5", "--enable-threadsafe --with-pthread=/usr/lib --enable-shared=no --enable-zlib=no" );
}

sub qt {
    my $baseFileName = "qt-all-opensource-src-4.5.2";
    prepareAndBuild( $baseFileName, "Qt", "-no-zlib -opensource -static -no-glib -fast -make libs -no-accessibility -no-qt3support -no-cups -no-qdbus -no-webkit" );
}

sub omni {
    genOmniScript();

    my $cmakeSettings = <<END;
CMAKE_BUILD_TYPE:STRING=Debug
CMAKE_INSTALL_PREFIX:STRING=$buildPath
QT_QMAKE_EXECUTABLE:STRING=$libPath/Qt/bin/qmake
DESIRED_QT_VERSION:STRING=4

Boost_INCLUDE_DIR:PATH=$libPath/Boost/include/boost-1_38/
Boost_LIBRARY_DIRS:FILEPATH=$libPath/Boost/
Boost_USE_MULTITHREADED:BOOL=ON

TIFF_INCLUDE_DIR:PATH=$libPath/libtiff/include
TIFF_LIBRARY:PATH=$libPath/libtiff/lib/libtiff.a

EXPAT_INCLUDE_DIR:PATH=$libPath/expat/include
EXPAT_LIBRARY:PATH=$libPath/expat/lib/libexpat.a

PNG_INCLUDE_DIR:PATH=$libPath/libpng/include
PNG_LIBRARY:PATH=$libPath/libpng/lib/libpng.a

END

    `echo "$cmakeSettings" > $omniPath/CMakeCache.txt`;
    `rm -rf $omniPath/CMakeFiles`;

    updateCMakeListsFile();

    `sh $scriptPath/buildomni.sh`;
}

sub updateCMakeListsFile {
    my $inFileName  = "$omniPath/CMakeLists.txt.template";
    my $outFileName = "$omniPath/CMakeLists.txt";

    open IN_FILE,  "<", $inFileName  or die "could not read $inFileName";
    open OUT_FILE, ">", $outFileName or die "could not read $outFileName";;

    while (my $line = <IN_FILE>) { 
	if( $line =~ /^SET\(OM_EXT_LIBS_DIR/ ) {
	    print OUT_FILE "SET(OM_EXT_LIBS_DIR \"$libPath\")\n";
	} elsif ( $line =~ /^SET\(BOOST_VERS_EXT/ ) {
	    my $fName = `ls $libPath/Boost/lib/libboost_system*38.a`;
	    ( my $boost_ver ) = ($fName =~/.*(gcc.*)\.a/);
	    if( isMac() ){
		$boost_ver = "x".$boost_ver;
	    } 
	    print OUT_FILE "SET(BOOST_VERS_EXT \"$boost_ver\")\n";
	} else {
	    print OUT_FILE $line;
	}
    }
    
    close OUT_FILE;
    close IN_FILE;
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
    libtiff();
}

# This is the official release option
sub release {
	my $version = "1.0";

	smallLibraries();
	boost();
	qt();
	vtk();
	omni();

	# Do release specific work now.

	# Cleanup any leftover cryptographic keys.
	`rm -rf omni/secret`;

	# FIXME: Need to tar up the right files
	print `tar -zcvf Omni1-release-bin-$version.tar.gz omni/bin/omni`;
}

sub menu {
    my $max_answer = 9;

    print "bootstrap.pl menu:\n";
    print "0 -- exit\n";
    print "1 -- Build small libs\n";
    print "2 -- Build boost\n";
    print "3 -- Build qt\n";
    print "4 -- Build vtk\n";
    print "5 -- Build omni\n";
    print "6 -- [Do 1 through 5]\n";
    print "7 -- Build one of the small libraries...\n";
    print "8 -- Generate scripts\n";
    print "9 -- Build and tar release!\n\n";

    while( 1 ){
	print "Please make selection: ";
	my $answer = <STDIN>;

	print "$answer\n";

	if( $answer =~ /^\d+$/ ) {
	    if( ($answer > -1) and ($answer < (1+$max_answer))){
		runMenuEntry( $answer );
		exit();
	    }
	}
    }
}

sub runMenuEntry {
    my $entry = $_[0];

    if( 0 == $entry ){
	return();
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
    }elsif( 7 == $entry ){
	smallLibraryMenu();
    }elsif( 8 == $entry ){
	genOmniScript();
    }elsif( 9 == $entry ){
        release();
    }
}

sub smallLibraryMenu() {
    my $max_answer = 6;

    print "build small library menu:\n";
    print "0 -- exit\n";
    print "1 -- Build expat\n";
    print "2 -- Build fontconfig\n";
    print "3 -- Build freetype\n";
    print "4 -- Build hdf5\n";
    print "5 -- Build libpng\n";
    print "6 -- Build libtiff\n\n";

    while( 1 ){
	print "Please make selection: ";
	my $answer = <STDIN>;

	print "$answer\n";

	if( $answer =~ /^\d+$/ ) {
	    if( ($answer > -1) and ($answer < (1+$max_answer))){
		runSmallLibraryMenuEntry( $answer );
		exit();
	    }
	}
    }
}

sub runSmallLibraryMenuEntry {
    my $entry = $_[0];

    if( 0 == $entry ){
        return();
    }elsif( 1 == $entry ){
        expat();
    }elsif( 2 == $entry ){
        fontconfig();
    }elsif( 3 == $entry ){
        freetype();
    }elsif( 4 == $entry ){
        hdf5();
    }elsif( 5 == $entry ){
        libpng();
    }elsif( 6 == $entry ){
        libtiff();
    }
}

sub checkCmdLineArgs {
    if ( 1 == @ARGV ) {
	runMenuEntry( @ARGV[0] );
    }
    else {
	menu();
    }
}

checkCmdLineArgs();

sub gcc {
    # just in case...
    # ../gcc-4.3.4/configure --prefix=/Users/purcaro/bin/gcc-4.3.4 --enable-languages=c,c++ --disable-nls
}
