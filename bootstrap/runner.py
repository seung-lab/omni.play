from string import Template
from library import Library
import os

from builder import Builder

class runner:
    ##
    # source tar-ball versions
    ##

    def __init__(self, numCores):
        self.numCores = numCores

    def makeBuilder(self, lib, buildOptions = ""):
        cwd = os.getcwd()
        b = Builder(cwd, lib, buildOptions)
        b.numCores = self.numCores
        return b

    def thrift(self):
        args = " ".join([ "CXXFLAGS='-g -O2'",
                          "CFLAGS='-g -O2'",
                          "--without-ruby",
                          "--without-erlang",
                          "--enable-gen-cpp",
                          "--with-boost={libPath}/Boost".format(libPath=self.libPath)
                          ])

        b = self.makeBuilder(THRIFT_VER, "thrift", THRIFT_URI, args)
        b.prepareAndBuild

        #    chdir(basePath."/external")
        #    # TODO: doesn't this path need to be done before building? (purcaro)
        #    print `patch -p0 < patches/thrift.patch`

    def libjpeg(self):
        b = self.makeBuilder(Library.jpeg())
        b.prepareAndBuild()

    def libpng(self):
        b = self.makeBuilder(Library.png())
        b.prepareAndBuild()

    def zlib(self):
        b = self.makeBuilder(Library.zlib())
        b.prepare()
        b.buildInSourceFolder()

    def boost(self):
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

        b = self.makeBuilder(Library.boost())
        b.prepareNukeSrcsFolder()

        bz = self.makeBuilder(Library.zlib())
        bz.wget()
        bz.untar()
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

    def omniServer(self):
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

    def printTitle(self, title):
        self.printLine()
        print title + ":\n"

    def printLine(self):
        print "\n**********************************************\n"
