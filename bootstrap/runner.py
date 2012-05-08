from string import Template
from library import LibraryMetadata
import os

from builder import Builder

class runner:
    def __init__(self, numCores):
        self.numCores = numCores

    def makeBuilder(self, lib):
        cwd = os.getcwd()
        b = Builder(cwd, lib)
        b.numCores = self.numCores
        return b

    def thrift(self):
        b = self.makeBuilder(LibraryMetadata.thrift())

        args = " ".join([ "CXXFLAGS='-g -O2'",
                          "CFLAGS='-g -O2'",
                          "--without-ruby",
                          "--without-erlang",
                          "--enable-gen-cpp",
                          "--with-boost={libs}/Boost".format(libs=b.libs_fp())
                          ])

        b.build_options(args)
        b.prepareAndBuild()

        ext_fp = b.ext_fp
        patch_fnp = os.path.join(ext_fp, "patches/thrift.patch")

        cmd = "/usr/bin/patch -d {ext_fp} -p0 -i {fnp}".format(ext_fp=ext_fp,
                                                               fnp=patch_fnp)
        print "patching using cmd: ", cmd
        os.system(cmd)

    def libjpeg(self):
        b = self.makeBuilder(LibraryMetadata.jpeg())
        b.prepareAndBuild()

    def libpng(self):
        b = self.makeBuilder(LibraryMetadata.png())
        b.prepareAndBuild()

    def libevent(self):
        b = self.makeBuilder(LibraryMetadata.event())
        b.prepareAndBuild()

    def zlib(self):
        b = self.makeBuilder(LibraryMetadata.zlib())
        b.prepare()
        b.buildInSourceFolder()

    def submodule(self):
    	print ("Initializing Submodules.")
    	os.system("git submodule init")
    	print ("Downloading Submodules.")
    	os.system("git submodule update")

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

        b = self.makeBuilder(LibraryMetadata.boost())
        b.prepareNukeSrcsFolder()

        bz = self.makeBuilder(LibraryMetadata.zlib())
        bz.wget()
        bz.untar()

        b.chdir_src()
        cmd = "./bootstrap.sh --prefix=" + b.lib_fp()
        cmd += " --with-libraries=filesystem,thread,system,iostreams,regex"

        print "configuring (" + cmd + ")\n"

        os.system(cmd)
        print "done\n"

        bjamFlags = "-j{num}".format(num=self.numCores)
        bjamFlags += " -sNO_BZIP2=1 -sZLIB_SOURCE=srcPath/ZLIB_VER"
        bjamFlags += " variant=release link=static threading=multi runtime-link=static"
        # bjamFlags += " toolset=gcc cxxflags=-std=gnu++0x"
        cmd = "./bjam " + bjamFlags + " install"
        print "building and installing (cmd)\n"
        os.system(cmd)
        b.chdir_home()
        print "done\n"


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
