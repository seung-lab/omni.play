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
my $omniExecPathMac  = $basePath.'/omni/bin/omni.app/Contents/MacOS/omni';

my $omniScriptFile = $scriptPath.'/buildomni.sh';
my $omniScriptOptions = "";

my $globalMakeOptions = "";

my $hostname = `hostname`;
my $profileOn = "";

my $NumCores;

##
# source tar-ball versions
##
my $BOOST_VER = "boost_1_46_1";
my $QT_VER = "qt-everywhere-opensource-src-4.7.3";
my $HDF5_VER = "hdf5-1.8.6";
my $ZLIB_VER = "zlib-1.2.5";
my $INTEL_TBB_VER = "tbb30_20110315oss";
my $THRIFT_VER = "thrift-0.6.1";
my $RE2C = "re2c-0.13.5";

#my $JPEG_VER = "libjpeg-turbo-1.1.0";
#if(isMac()){
#    $JPEG_VER = "jpeg-8c";
#}
my $JPEG_VER = "jpeg-8c";

# from http://stackoverflow.com/questions/334686/how-can-i-detect-the-operating-system-in-perl
sub isMac {
    return ("darwin" eq $^O);
}

sub getMacOSXversionNum
{
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

sub checkForMac
{
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
#    return ($hostname =~ /brainiac/);
    return 0;
}

sub dealWithCluster
{
    if ( onCluster() ) {
	my $nodes = `cat $basePath/scripts/cluster/distcchosts`;
	$ENV{DISTCC_HOSTS} = $nodes;
	$globalMakeOptions .= " CC=\"distcc /usr/local/gcc-4.3.4/bin/gcc4.3.4\" " .
	    " CXX=\"distcc /usr/local/gcc-4.3.4/bin/g++4.3.4\"";
	$profileOn = "";
    }
}

sub makeDirPaths
{
    if( !-e $buildPath && !-l $buildPath){
	mkpath($buildPath) or die "could not create $buildPath";
    }

    if( !-e $srcPath && !-l $srcPath){
	mkpath($srcPath) or die "could not create $srcPath";
    }
}

sub genOmniScript
{
    my $script = "";
    $script .= "cd $basePath/omni;";

    if(@_ == 1) {
	my $debugMode = $_[0];
	if($debugMode){
	    print "building omni in debug mode\n";
	    $script .= "rm -f release;";
	} else {
	    print "building omni in release mode\n";
	    $script .= "touch release;";
	}
    } else {
	print "building omni with current make options\n";
    }

    $script .= "../external/libs/Qt/bin/qmake omni.pro;";

    if(@_ == 1) {
	print "will make clean\n";
	$script .= "make clean;";
    }

    $script .= "make $globalMakeOptions";

    if(isMac()) {
        my $nigOutPath = "$basePath/omni/bin/omni.app/Contents/Resources/";
	$script .= "; cp -r ../external/$QT_VER/src/gui/mac/qt_menu.nib $nigOutPath\n";
    }

    open (SCRIPT, ">", $omniScriptFile) or die $!;
    print SCRIPT $script;
    close SCRIPT;
    `chmod +x $omniScriptFile`;
}

sub setupBuildFolder
{
    my $baseFileName = $_[0];

    print "==> creating new build folder...";
    `mkdir $buildPath/$baseFileName` if (!-e "$buildPath/$baseFileName" );
    print "done\n";
}

sub nukeSrcsFolder
{
    my $baseFileName = $_[0];

    print "==> removing old srcs folder...";
    `rm -rf $srcPath/$baseFileName`;
    print "done\n";
}

sub nukeBuildFolder
{
    my $baseFileName = $_[0];

    print "==> removing old build folder...";
    `rm -rf $buildPath/$baseFileName`;
    print "done\n";
}

sub nukeLibraryFolder
{
    my $libFolderName = $_[0];

    print "==> removing old library folder...";
    `rm -rf $libPath/$libFolderName`;
    print "done\n";
}

sub untar
{
    my $baseFileName = $_[0];

    if (-e "$srcPath/$baseFileName" ){
	print "==> skipping untar\n";
	return;
    }

    my $tarOptions = " -C $srcPath/ ";

    print "==> untarring to external/srcs/...";
    `tar -zxf $tarballPath/$baseFileName.tar.gz $tarOptions`;
    print "done\n";
}

sub prepare
{
    my $baseFileName  = $_[0];
    my $libFolderName = $_[1];

    printTitle(        $baseFileName );
    nukeBuildFolder(   $baseFileName );
    nukeLibraryFolder( $libFolderName );
    untar(             $baseFileName );
    setupBuildFolder(  $baseFileName );
}

sub prepareNukeSrcsFolder
{
    my $baseFileName  = $_[0];
    my $libFolderName = $_[1];

    printTitle(        $baseFileName );
    nukeSrcsFolder(    $baseFileName );
    nukeBuildFolder(   $baseFileName );
    nukeLibraryFolder( $libFolderName );
    untar(             $baseFileName );
    setupBuildFolder(  $baseFileName );
}

sub build
{
    my $baseFileName  = $_[0];
    my $libFolderName = $_[1];
    my $buildOptions  = $_[2];

    chdir( "$buildPath/$baseFileName" );

    configure(   $baseFileName, $libFolderName, $buildOptions );
    make();
    makeInstall();

    chdir( $basePath );
}

sub buildInSourceFolder
{
    my $baseFileName  = $_[0];
    my $libFolderName = $_[1];
    my $buildOptions  = $_[2];

    chdir( "$srcPath/$baseFileName" );

    configure(   $baseFileName, $libFolderName, $buildOptions );
    make();
    makeInstall();

    chdir( $basePath );
}

sub configure
{
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

sub make
{
    my $cmd = "make $globalMakeOptions";
    print "==> running make...";
    print "($cmd)\n";
    # TODO: check return values; die if something went wrong...
    print `($cmd)`;
    print "done with make\n\n";
}

sub makeInstall
{
    my $cmd = "make install";
    print "==> running make install...";
    print "($cmd)\n";
    # TODO: check return values; die if something went wrong...
    print `($cmd)`;
    print "done with make install\n";
}

sub prepareAndBuild
{
    my $baseFileName  = $_[0];
    my $libFolderName = $_[1];

    my $buildOptions = "";
    if( scalar(@_) > 2 ) {
	$buildOptions = $_[2];
    }

    prepare( $baseFileName, $libFolderName );
    build(   $baseFileName, $libFolderName, $buildOptions );
}

sub prepareNukeSrcsAndBuild
{
    my $baseFileName  = $_[0];
    my $libFolderName = $_[1];

    my $buildOptions = "";
    if( scalar(@_) > 2 ) {
	$buildOptions = $_[2];
    }

    prepareNukeSrcsFolder( $baseFileName, $libFolderName );
    build(   $baseFileName, $libFolderName, $buildOptions );
}

sub thrift
{
#    prepareAndBuild( $RE2C, "re2c");

    my @argsList = qw( CXXFLAGS='-g -O2' CFLAGS='-g -O2'
--without-ruby
--without-erlang
--enable-gen-cpp
);

    my $args = concatStrList(@argsList);
    $args .= " --with-boost=$libPath/Boost";

    prepareAndBuild( $THRIFT_VER, "thrift", $args);
}

sub libjpeg
{
    prepareAndBuild( $JPEG_VER, "libjpeg" );
}

sub intel_tbb
{
    prepareAndBuild( $INTEL_TBB_VER, "intel_tbb" );
}

sub hdf5
{
    my $args = " --enable-threadsafe ";
    if( isWindowsNative() || isWindowsCygwin() ){

    } else {
	$args .= " --with-pthread=/usr/lib ";
    }

    prepareAndBuild( $HDF5_VER, "HDF5", $args );
}

sub boost
{
#./bjam --show-libraries
    #The following libraries require building:
    #- date_time
    #- filesystem
    #- graph
    #- graph_parallel
    #- iostreams
    #- math
    #- mpi
    #- program_options
    #- python
    #- random
    #- regex
    #- serialization
    #- signals
    #- system
    #- test
    #- thread
    #- wave

    my $baseFileName = $BOOST_VER;
    my $libFolderName = "Boost";
    prepareNukeSrcsFolder( $baseFileName, $libFolderName );

    untar($ZLIB_VER);

    my $cmd = "cd $srcPath/$baseFileName; ./bootstrap.sh --prefix=$libPath/$libFolderName ";
    $cmd .= " --with-libraries=filesystem,thread,system,iostreams";

    print "configuring ($cmd)\n";

    `($cmd)`;
    print "done\n";

    my $bjamFlags = "-j$NumCores ";
    $bjamFlags .= " -sNO_BZIP2=1 -sZLIB_SOURCE=$srcPath/$ZLIB_VER ";
    $bjamFlags .= " --disable-filesystem2 ";
    $cmd = "cd $srcPath/$baseFileName; ./bjam $bjamFlags install";
    print "building and installing ($cmd)\n";
    `($cmd)`;
    print "done\n";
}

sub qt
{
# new qt buidls has several messages:
    # requires zlib;
    # suggests --no-excpetion to reduce gcc-induced memory footprint increases
    # disable postgres/sqlite
    # debug not enabled?
    my $baseFileName = $QT_VER;
    my @argsList = qw( -release -opensource -no-glib -v
 -no-exceptions
 -no-fast -make libs -make tools
 -no-accessibility -no-qt3support -no-cups -no-qdbus -no-webkit
 -no-sql-sqlite -no-xmlpatterns -no-phonon -no-phonon-backend
 -no-svg -qt-zlib -qt-gif -qt-libtiff -qt-libpng -no-libmng
 -qt-libjpeg -no-openssl -no-nis -no-cups -no-iconv -no-freetype
 -no-multimedia -no-javascript-jit -no-script -no-scripttools
);

    my $args = concatStrList(@argsList);

    if ( isMacSnowLeopard() ){
	$args .= " -arch x86_64 ";
    } else {
	$args .= " -optimized-qmake ";
    }

    prepareAndBuild( $baseFileName, "Qt", $args );
}

sub omni
{
    printTitle("omni");
    genOmniScript(@_);

    if(isMac()){
	unlink($omniExecPathMac);
    }

    my $cmd = "sh $omniScriptFile";
    print "running: ($cmd)\n";
    print `$cmd`;
    print "done\n";
}

sub printTitle
{
    my $title = $_[0];
    printLine();
    print $title.":\n";
}

sub printLine {
    print "\n**********************************************\n";
}

sub menu
{
    print "bootstrap.pl menu:\n";
    print "0 -- exit\n";
    print "1 -- Build hdf5\n";
    print "2 -- Build boost\n";
    print "3 -- Build qt\n";
    print "4 -- Build libjpeg\n";
    print "5 -- Build omni\n";
    print "6 -- [Do 1 through 5]\n";
    print "7 -- Build thrift\n";
    print "8 -- Generate scripts\n";
    print "10 -- Experimental builds...\n";
    print "11 -- Ubuntu library apt-gets...\n";
    print "12 -- build omni (make clean first)...\n";
    print "13 -- build omni in debug mode...\n";
    print "14 -- build omni in release mode...\n\n";
    my $max_answer = 14;

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

sub buildAll
{
    hdf5();
    boost();
    qt();
    libjpeg();
    thrift();
    omni();
}

sub runMenuEntry
{
    my $entry = $_[0];

    if( 0 == $entry ){
	return();
    }elsif( 1 == $entry ){
	hdf5();
    }elsif( 2 == $entry ){
        boost();
    }elsif( 3 == $entry ){
	qt();
    }elsif( 4 == $entry ){
        libjpeg();
    }elsif( 5 == $entry ){
	omni();
    }elsif( 6 == $entry ){
	buildAll();
    }elsif( 7 == $entry ){
        thrift();
    }elsif( 8 == $entry ){
	genOmniScript();
    }elsif( 9 == $entry ){
    }elsif( 10 == $entry ){
        experimentalMenu();
    }elsif( 11 == $entry ){
        doUbuntuAptGets();
    }elsif( 12 == $entry ){
        omniClean();
    }elsif( 13 == $entry ){
        omni(1);
    }elsif( 14 == $entry ){
        omni(0);
    }
}

sub numberOfCores
{
    $NumCores = 2;
    if (-e "/proc/cpuinfo") {
	$NumCores =`cat /proc/cpuinfo  | grep processor | wc -l`;
    }

    if( isMac() ){
	my $numCoreStr = `/usr/sbin/system_profiler SPHardwareDataType | grep 'Total Number Of Cores'`;
	if ($numCoreStr =~ m/.*:.*([\d*])/) {
	    $NumCores = $1;
	}
    }

    if( $NumCores < 2 ){
	$NumCores = 2;
    }

    return $NumCores;
}

sub setupParallelBuildOption
{
    $NumCores = numberOfCores();
    if( scalar(@_) > 0 ) {
	$NumCores = $_[0];
    }
    if (onCluster()) {
        $NumCores = 60;
    }

    chomp($NumCores);

    $globalMakeOptions .=  " -j$NumCores ";

    print "number of parallel builds (override with \"-j n\" switch to bootstrap.pl): $NumCores\n";
}

sub experimentalMenu
{
    print "experimental build menu:\n";
    print "0 -- exit\n";
    print "1 -- Build Omni no debug\n";
    print "2 -- Build qt 4.7\n";
    print "3 -- Build Intel TBB\n";
    print "\n";
    my $max_answer = 3;

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

sub runExperimentalMenuEntry
{
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
	qt();
    } elsif( 3 == $entry ){
	intel_tbb();
    }
}

sub checkCmdLineArgs
{
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

sub doUbuntuAptGets
{
    my @packages = qw( libxrender-dev libxext-dev freeglut3-dev g++
	libfreetype6-dev libxml2 libxml2-dev mesa-common-dev
	libxt-dev libgl1-mesa-dev libglu1-mesa-dev libgl1-mesa-dri-dbg
	libgl1-mesa-glx-dbg);

    my $args = concatStrList(@packages);

    print `sudo apt-get -y install $args`;
    print "Done with the Ubuntu apt-gets! \n\n";
}

sub concatStrList
{
    my (@array) = @_;
    my $args = "";
    foreach (@array){
	$args .= " $_";
    }
    return $args;
}

sub omniClean
{
    `(cd $omniPath; make clean)`;
    omni();
}

checkForMac();
dealWithCluster();
makeDirPaths();
checkCmdLineArgs();
