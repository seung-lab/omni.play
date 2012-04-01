#!/usr/bin/python

import os

NumCores

##
# source tar-ball versions
##
BOOST_VER = "boost_1_49_0"
BOOST_URI = "http://sourceforge.net/projects/boost/files/boost/1.49.0/boost_1_49_0.tar.gz/download"
ZLIB_VER = "zlib-1.2.6"
ZLIB_URI = "http://zlib.net/zlib-1.2.6.tar.gz"
THRIFT_VER = "thrift-0.8.0"
THRIFT_URI = "http://www.eng.lsu.edu/mirrors/apache/thrift/0.8.0/thrift-0.8.0.tar.gz"
JPEG_VER = "libjpeg-turbo-1.2.0"
JPEG_URI = "http://sourceforge.net/projects/libjpeg-turbo/files/latest/download"
PNG_VER = "libpng-1.5.9"
PNG_URI = "http://prdownloads.sourceforge.net/libpng/libpng-1.5.9.tar.gz?download"


def makeBuilder():
    cwd = os.getcwd()
    b = builder(cwd

def thrift():
#    prepareAndBuild( RE2C, "re2c")

    @argsList = qw( CXXFLAGS='-g -O2' CFLAGS='-g -O2'
--without-ruby
--without-erlang
--enable-gen-cpp
)

    args = concatStrList(@argsList)
    args .= " --with-boost=libPath/Boost"

    prepareAndBuild( THRIFT_VER, "thrift", THRIFT_URI, args)
    chdir(basePath."/external")
    print `patch -p0 < patches/thrift.patch`


def libjpeg():
    prepareAndBuild( JPEG_VER, "libjpeg", JPEG_URI)


def libpng():
    prepareAndBuild( PNG_VER, "libpng", PNG_URI )


def zlib():
    prepare( ZLIB_VER, "zlib" )
    buildInSourceFolder( ZLIB_VER, "zlib", ZLIB_URI )


def boost():
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

    baseFileName = BOOST_VER
    libFolderName = "Boost"
    prepareNukeSrcsFolder( baseFileName, libFolderName, BOOST_URI)

	wget(ZLIB_URI, ZLIB_VER)
    untar(ZLIB_VER)

    cmd = "cd srcPath/baseFileName ./bootstrap.sh --prefix=libPath/libFolderName "
    cmd .= " --with-libraries=filesystem,thread,system,iostreams,regex"

    print "configuring (cmd)\n"

    `(cmd)`
    print "done\n"

    bjamFlags = "-jNumCores"
    bjamFlags .= " -sNO_BZIP2=1 -sZLIB_SOURCE=srcPath/ZLIB_VER"
    bjamFlags .= " variant=release link=static threading=multi runtime-link=static"
    # bjamFlags .= " toolset=gcc cxxflags=-std=gnu++0x"
    cmd = "cd srcPath/baseFileName ./bjam bjamFlags install"
    print "building and installing (cmd)\n"
    `(cmd)`
    print "done\n"


def omniServer():
    printTitle("omni.server")
    genOmniScript(@_)

    if(isMac()){
	unlink(omniExecPathMac)
    

    cmd = "sh omniScriptFile"
    print "running: (cmd)\n"
    print `cmd`
    print "done\n"


def printTitle():
    title = _[0]
    printLine()
    print title.":\n"


def printLine():
    print "\n**********************************************\n"


def menu():
    print "bootstrap.pl menu:\n"
    print "0 -- exit\n"
    print "1 -- Build All\n"
    print "2 -- Build boost\n"
    print "3 -- Build thrift\n"
    print "4 -- Build libjpeg\n"
    print "5 -- Build libpng\n"
    max_answer = 12

    while( 1 ){
	print "Please make selection: "
	answer = <STDIN>

	if( answer && answer =~ /^\d+/ ):
	    if( (answer > -1) and (answer < (1+max_answer))){
		runMenuEntry( answer )
		exit()
	    
	
    


def buildAll():
    boost()
    thrift()
    libjpeg()
    libpng()


def runMenuEntry():
    entry = _[0]

    if( 0 == entry ){
	return()
    elsif( 1 == entry ){
        buildAll()
    elsif( 2 == entry ){
        boost()
    elsif( 3 == entry ){
        thrift()
    elsif( 4 == entry ){
        libjpeg()
    elsif( 5 == entry ){
        libpng()
    elsif( 6 == entry ){
        zlib()
    


def numberOfCores():
    NumCores = 2
    if (-e "/proc/cpuinfo"):
	NumCores =`cat /proc/cpuinfo  | grep processor | wc -l`
    

    if( isMac() ){
	numCoreStr = `/usr/sbin/system_profiler SPHardwareDataType | grep 'Total Number Of Cores'`
	if (numCoreStr =~ m/.*:.*([\d*])/):
	    NumCores = 1
	
    

    if( NumCores < 2 ){
	NumCores = 2
    

    return NumCores


def setupParallelBuildOption():
    NumCores = numberOfCores()
    if( scalar(@_) > 0 ):
	NumCores = _[0]
    
    if (onCluster()):
        NumCores = 60
    

    chomp(NumCores)

    globalMakeOptions .=  " -jNumCores "

    print "number of parallel builds (override with \"-j n\" switch to bootstrap.pl): NumCores\n"


def checkCmdLineArgs():
    if ( 1 == @ARGV ):
	setupParallelBuildOption()
	runMenuEntry( ARGV[0] )
     elsif (2 == @ARGV ):
	setupParallelBuildOption( ARGV[1] )
	menu()
     else:
	setupParallelBuildOption()
	menu()
    


def doUbuntuAptGets():
    @packages = qw( libxrender-dev libxext-dev freeglut3-dev g++
	libfreetype6-dev libxml2 libxml2-dev mesa-common-dev
	libxt-dev libgl1-mesa-dev libglu1-mesa-dev libgl1-mesa-dri-dbg
	libgl1-mesa-glx-dbg libncurses5-dev)

    args = concatStrList(@packages)

    print `sudo apt-get -y install args`
    print "Done with the Ubuntu apt-gets! \n\n"


def concatStrList():
    (@array) = @_
    args = ""
    foreach (@array){
	args .= " _"
    
    return args


def omniClean():
    `(cd omniPath make clean)`
    omniServer()


checkForMac()
dealWithCluster()
makeDirPaths()
checkCmdLineArgs()
