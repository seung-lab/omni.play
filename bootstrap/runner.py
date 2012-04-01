from string import Template
import os

numCores = 2

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


def makeBuilder(baseFileName, libFolderName, uri):
    cwd = os.getcwd()
    b = builder(cwd, baseFileName, libFolderName, uri)
    b.numCores = 2
    return b

def thrift():
    args = " ".join([ "CXXFLAGS='-g -O2'",
                      "CFLAGS='-g -O2'",
                      "--without-ruby",
                      "--without-erlang",
                      "--enable-gen-cpp",
                      "--with-boost={libPath}/Boost".format(libPath=self.libPath)
                      ])
                
    b = makeBuilder(THRIFT_VER, "thrift", THRIFT_URI, args)
    b.prepareAndBuild
    
    #    chdir(basePath."/external")
    #    # TODO: doesn't this path need to be done before building? (purcaro)
    #    print `patch -p0 < patches/thrift.patch`

def libjpeg():
    b = makeBuilder( JPEG_VER, "libjpeg", JPEG_URI )
    b.prepareAndBuild
    
def libpng():
    b = makeBuilder( PNG_VER, "libpng", PNG_URI )
    b.prepareAndBuild

def zlib():
    b = makeBuilder( ZLIB_VER, "zlib", ZLIB_URI )
    b.prepare
    b.buildInSourceFolder

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

    b = makeBuilder( BOOST_VER, "Boost", BOOST_URI )
    b.prepareNukeSrcsFolder
    
    bz = makeBuilder( ZLIB_VER, "zlib", ZLIB_URI )
    bz.wget
    bz.untar
"""
    cmd = "cd srcPath/baseFileName ./bootstrap.sh --prefix=libPath/libFolderName "
    cmd += " --with-libraries=filesystem,thread,system,iostreams,regex"

    print "configuring (cmd)\n"

    `(cmd)`
    print "done\n"

    bjamFlags = "-jNumCores"
    bjamFlags += " -sNO_BZIP2=1 -sZLIB_SOURCE=srcPath/ZLIB_VER"
    bjamFlags += " variant=release link=static threading=multi runtime-link=static"
    # bjamFlags += " toolset=gcc cxxflags=-std=gnu++0x"
    cmd = "cd srcPath/baseFileName ./bjam bjamFlags install"
    print "building and installing (cmd)\n"
    `(cmd)`
    print "done\n"
"""

def omniServer():
    printTitle("omni.server")
"""
    genOmniScript(@_)

    if(isMac()){
	unlink(omniExecPathMac)
    

    cmd = "sh omniScriptFile"
    print "running: (cmd)\n"
    print os.system(cmd)
    print "done\n"
"""

def printTitle(title):
    printLine()
    print title + ":\n"

def printLine():
    print "\n**********************************************\n"
