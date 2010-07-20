#!/usr/bin/perl -w

# NOTE: build process will be done "out-of-source". 
#  (from vtk): "When your build generates files, they have to go somewhere. 
#   An in-source build puts them in your source tree. 
#   An out-of-source build puts them in a completely separate directory, 
#   so that your source tree is unchanged."
# This allows us to untar only once, but build multiple times--much faster
#  on older computers!

use strict;
use File::Path;

my $basePath = `pwd`;
chomp $basePath;
my $buildPath   = $basePath.'/external/builds';
my $srcPath     = $basePath.'/external/srcs';
my $libPath     = $basePath.'/external/libs';
my $tarballPath = $basePath.'/external/tarballs';
my $scriptPath  = $basePath.'/scripts';
my $omniPath    = $basePath.'/omni';

my $omniScriptFile = $scriptPath.'/buildomni.sh';
my $omniScriptOptions = "";

my $vtkScriptFile = $scriptPath.'/buildvtk.sh';

my $globalMakeOptions = "";

my $hostname = `hostname`;
my $profileOn = "";

# from http://stackoverflow.com/questions/334686/how-can-i-detect-the-operating-system-in-perl
sub isMac {
    return ("darwin" eq $^O);
}

sub getMacOSXversionNum {
    my $versionStr = `/usr/sbin/system_profiler SPSoftwareDataType | grep 'System Version'`;
    my $ret = 0;
    if ($versionStr =~ m/.*10\.([\d])\..*/){
	$ret = $1;
    }
    return $ret;
}

sub isMacLeopard {
    return isMac() && (5 == getMacOSXversionNum());
}

sub isMacSnowLeopard {
    return isMac() && (6 == getMacOSXversionNum());
}

sub checkForMac {
    if(isMac()){
	print "Mac OS X version is: ".getMacOSXversionNum();
	if(isMacLeopard()){
	    print " (Leopard)";
	}elsif(isMacSnowLeopard()){
	    print " (Snow Leopard)";
	}else{
	    print " (unknown)";
	}
	print "\n";
    }
}

sub isLinux {
    return ("linux" eq $^O);
}

sub isWindowsNative {
    return ("MSWin32" eq $^O);
}

sub isWindowsCygwin {
    return ("cygwin" eq $^O);
}

sub onCluster {
    return ($hostname =~ /brainiac/);
}

sub dealWithCluster {
    if ( onCluster() ) {
	my $nodes = `cat $basePath/scripts/cluster/distcchosts`;
	$ENV{DISTCC_HOSTS} = $nodes;
	$globalMakeOptions .= " CC=\"distcc /usr/local/gcc-4.3.4/bin/gcc4.3.4\" " .
	    " CXX=\"distcc /usr/local/gcc-4.3.4/bin/g++4.3.4\"";
	$profileOn = "";
    }
}

sub makeDirPaths {
    if( !-e $buildPath && !-l $buildPath){
	mkpath($buildPath) or die "could not create $buildPath";
    }

    if( !-e $srcPath && !-l $srcPath){
	mkpath($srcPath) or die "could not create $srcPath";
    }
}

sub genOmniScript {
    open (SCRIPT, ">", $omniScriptFile) or die $!;

    my $script = <<END;
cd $basePath/omni
../external/libs/Qt/bin/qmake omni.pro
make $globalMakeOptions
END

    if(isMac()) {
        $script .= "cp -r ../external/srcs/qt-everywhere-opensource-src-4.7.0-beta2/src/gui/mac/qt_menu.nib $basePath/omni/bin/omni.app/Contents/Resources/\n";
    }

    print SCRIPT $script;
    close SCRIPT;
    `chmod +x $omniScriptFile`;
}

sub genVTKscript {
    my $baseFileName = $_[0];
    open (SCRIPT, ">", $vtkScriptFile) or die $!;

    my $makeOps = $globalMakeOptions;
    if ( isMac() ){
	$makeOps = " -k ";
    }
    
    my $script = <<END;
cd $buildPath/$baseFileName
cmake $srcPath/$baseFileName
make $makeOps
make install    
END
    print SCRIPT $script;
    close SCRIPT;
    `chmod +x $vtkScriptFile`;
}

sub vtk {
    my $baseFileName = "vtk-5.4.2";
    genVTKscript( $baseFileName );
    prepareNukeSrcsFolder( $baseFileName, "VTK" );

    # Work around for a bug in the VTK tar ball.
    `chmod 600 $srcPath/$baseFileName/Utilities/vtktiff/tif_fax3sm.c`;
    
    `echo "CMAKE_INSTALL_PREFIX:PATH=$libPath/VTK/" >> $buildPath/$baseFileName/CMakeCache.txt`;
    if ( isMacLeopard() ){
	`echo "BUILD_SHARED_LIBS:BOOL=ON" >> $buildPath/$baseFileName/CMakeCache.txt`;
    } else {
        `echo "BUILD_SHARED_LIBS:BOOL=OFF" >> $buildPath/$baseFileName/CMakeCache.txt`;
    }
    `echo "CMAKE_BUILD_TYPE:STRING=Debug" >> $buildPath/$baseFileName/CMakeCache.txt`;
    `echo "CMAKE_CXX_FLAGS_DEBUG:STRING=-${profileOn}g -Wall -I $libPath/libtiff/include" >> $buildPath/$baseFileName/CMakeCache.txt`;
    `echo "BUILD_TESTING:BOOL=OFF" >> $buildPath/$baseFileName/CMakeCache.txt`;

    #`patch $srcPath/$baseFileName/Utilities/MaterialLibrary/ProcessShader.cxx -i $basePath/external/patches/vtk-processshader.patch`;

    my $cmd = "sh $vtkScriptFile";
    print "running: ($cmd)\n";
    `$cmd`;
    print "done\n";
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

sub prepareNukeSrcsAndBuild {
    my $baseFileName  = $_[0];
    my $libFolderName = $_[1];

    my $buildOptions = "";
    if( scalar(@_) > 2 ) {
	$buildOptions = $_[2];
    }

    prepareNukeSrcsFolder( $baseFileName, $libFolderName );
    build(   $baseFileName, $libFolderName, $buildOptions );
}

sub libpng {
    prepareAndBuild( "libpng-1.2.39", "libpng" );
}

sub libtiff { 
    prepareNukeSrcsFolder( "tiff-3.8.2", "libtiff" );
    buildInSourceFolder( "tiff-3.8.2", "libtiff", "--without-jpeg" );
}

sub freetype {
    my $tempMakeOptions = $globalMakeOptions;
    $globalMakeOptions = "";
    prepareAndBuild( "freetype-2.3.9", "FreeType" );
    $globalMakeOptions = $tempMakeOptions;
}

sub fontconfig {
    prepareNukeSrcsAndBuild( "fontconfig-2.7.1", "Fontconfig" );
}

sub expat {
    prepareAndBuild( "expat-2.0.1", "expat" );
}

sub hdf5 {
    hdf5_18();
}

sub hdf5_18 {
    my $args = " --enable-threadsafe ";
    if( isWindowsNative() || isWindowsCygwin() ){

    } else {
	$args .= " --with-pthread=/usr/lib ";
    }

    prepareAndBuild( "hdf5-1.8.4-patch1", "HDF5", $args );
}

sub qt {
    if(isMac()){
	qt47();
    } else {
	qt46();
    }
}

sub qt46 {
    # new qt buidls has several messages:
    # requires zlib; 
    # suggests --no-excpetion to reduce gcc-induced memory footprint increases
    # disable postgres/sqlite
    # debug not enabled?
    my $baseFileName = "qt-everywhere-opensource-src-4.6.2";
    my $args = "-debug -opensource -no-glib -fast -make libs -make tools -no-accessibility -no-qt3support -no-cups -no-qdbus -no-webkit -no-sql-sqlite -no-xmlpatterns -no-phonon -no-phonon-backend -no-svg -qt-zlib -qt-gif -qt-libtiff -qt-libpng -no-libmng -qt-libjpeg -no-openssl -no-nis -no-cups -no-iconv -no-dbus -no-freetype";
    if ( isMacSnowLeopard() ){
	$args .= " -arch x86_64 ";
    }

    prepareAndBuild( $baseFileName, "Qt", $args );
}

sub qt47 {
    # new qt buidls has several messages:
    # requires zlib;
    # suggests --no-excpetion to reduce gcc-induced memory footprint increases
    # disable postgres/sqlite
    # debug not enabled?
    my $baseFileName = "qt-everywhere-opensource-src-4.7.0-beta2";
    my $args = "-debug -no-framework -opensource -no-glib -fast -make libs -make tools -no-accessibility -no-qt3support -no-cups -no-qdbus -no-webkit -no-sql-sqlite -no-xmlpatterns -no-phonon -no-phonon-backend -no-svg -qt-zlib -qt-gif -qt-libtiff -qt-libpng -no-libmng -qt-libjpeg -no-openssl -no-nis -no-cups -no-iconv -no-dbus -no-freetype";
    if ( isMacSnowLeopard() ){
        $args .= " -arch x86_64 ";
    }

    prepareAndBuild( $baseFileName, "Qt", $args );
}

sub apacheThrift {
    prepareAndBuild( "thrift-0.2.0", "thrift");
}

sub omni {
    printTitle("omni");
    genOmniScript();

    my $cmd = "sh $omniScriptFile";
    print "running: ($cmd)\n";
    print `$cmd`;
    print "done\n";
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

	buildAll();

	# Do release specific work now.

	# Cleanup any leftover cryptographic keys.
	`rm -rf omni/secret`;

	# FIXME: Need to tar up the right files
	print `tar -zcvf Omni1-release-bin-$version.tar.gz omni/bin/omni`;
}

sub menu {
    print "bootstrap.pl menu:\n";
    print "0 -- exit\n";
    print "1 -- Build small libs\n";
    print "2 -- Build Apache Thrift\n";
    print "3 -- Build qt\n";
    print "4 -- Build vtk\n";
    print "5 -- Build omni\n";
    print "6 -- [Do 1 through 5]\n";
    print "7 -- Build one of the small libraries...\n";
    print "8 -- Generate scripts\n";
    print "9 -- Build and tar release!\n";
    print "10 -- Experimental builds...\n";
    print "11 -- Ubuntu library apt-gets...\n\n";
    my $max_answer = 11;

    while( 1 ){
	print "Please make selection: ";
	my $answer = <STDIN>;

	if( $answer && $answer =~ /^\d+$/ ) {
	    if( ($answer > -1) and ($answer < (1+$max_answer))){
		runMenuEntry( $answer );
		exit();
	    }
	}
    }
}

sub buildAll {
	smallLibraries();
	apacheThrift();
	qt();
	vtk();
	omni();
}

sub runMenuEntry {
    my $entry = $_[0];

    if( 0 == $entry ){
	return();
    }elsif( 1 == $entry ){
	smallLibraries();
    }elsif( 2 == $entry ){
	apacheThrift();
    }elsif( 3 == $entry ){
	qt();
    }elsif( 4 == $entry ){
	vtk();
    }elsif( 5 == $entry ){
	omni();
    }elsif( 6 == $entry ){
	buildAll();
    }elsif( 7 == $entry ){
	smallLibraryMenu();
    }elsif( 8 == $entry ){
	genOmniScript();
    }elsif( 9 == $entry ){
        release();
    }elsif( 10 == $entry ){
        experimentalMenu();
    }elsif( 11 == $entry ){
        doUbuntuAptGets();
    }
}

sub smallLibraryMenu() {
    print "build small library menu:\n";
    print "0 -- exit\n";
    print "1 -- Build expat\n";
    print "2 -- Build fontconfig\n";
    print "3 -- Build freetype\n";
    print "4 -- Build hdf5\n";
    print "5 -- Build libpng\n";
    print "6 -- Build libtiff\n\n";
    my $max_answer = 6;

    while( 1 ){
	print "Please make selection: ";
	my $answer = <STDIN>;

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

sub numberOfCores {
    
    my $numCores = 2;
    if (-e "/proc/cpuinfo") {
	$numCores =`cat /proc/cpuinfo  | grep processor | wc -l`;
    }

    if( isMac() ){
	my $numCoreStr = `/usr/sbin/system_profiler SPHardwareDataType | grep 'Total Number Of Cores'`;
	if ($numCoreStr =~ m/.*:.*([\d*])/) {
	    $numCores = $1;
	}
    }

    if( $numCores < 2 ){
	$numCores = 2;
    }

    return $numCores-1;
}

sub setupParallelBuildOption {

    my $numCores = numberOfCores();
    if( scalar(@_) > 0 ) {
	$numCores = $_[0];
    }

    if (onCluster()) {
        $globalMakeOptions .=  " -j60 ";
    } else {
        $globalMakeOptions .=  " -j$numCores ";
    }

    print "number of parallel builds (override with \"-j n\" switch to bootstrap.pl): $numCores\n";
}

sub experimentalMenu {
    print "experimental build menu:\n";
    print "0 -- exit\n";
    print "1 -- Build Omni no debug\n";
    print "2 -- Build qt 4.7\n";
    print "\n";
    my $max_answer = 2;

    while( 1 ){
	print "Please make selection: ";
	my $answer = <STDIN>;

	if( $answer =~ /^\d+$/ ) {
	    if( ($answer > -1) and ($answer < (1+$max_answer))){
		runExperimentalMenuEntry( $answer );
		exit();
	    }
	}
    }
}

sub runExperimentalMenuEntry {
    my $entry = $_[0];

    if( 0 == $entry ){
        return();
    }elsif( 1 == $entry ){
	`(cd $omniPath; make clean)`;
	$omniScriptOptions = " -D NO_DEBUG=1 ";
	$globalMakeOptions .= " VERBOSE=1 ";
	genOmniScript();
	omni();
    } elsif( 2 == $entry ){
	qt47();
    }
}

sub checkCmdLineArgs {
    if ( 1 == @ARGV ) {
	setupParallelBuildOption();
	runMenuEntry( $ARGV[0] );
    } elsif (2 == @ARGV ) {
	setupParallelBuildOption( $ARGV[1] );
	menu();
    } else {
	setupParallelBuildOption();
	menu();
    }
}

sub doUbuntuAptGets{
    my @packages = qw( libxrender-dev libxext-dev freeglut3-dev g++ 
	libfreetype6-dev libxml2 libxml2-dev cmake mesa-common-dev 
	libxt-dev libgl1-mesa-dev libglu1-mesa-dev libgl1-mesa-dri-dbg
	libgl1-mesa-glx-dbg libboost-dev flex bison);

    my $args = "";
    foreach (@packages){
	$args .= " $_";
    }

    print `sudo apt-get -y install $args`;
    print "Done with the Ubuntu apt-gets! \n\n";
}

checkForMac();
dealWithCluster();
makeDirPaths();
checkCmdLineArgs();
