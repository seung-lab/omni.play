import fileutils
from string import Template
from library import LibraryMetadata
import os

from builder import Builder

class runner:
    def __init__(self, num_cores):
        self.num_cores = num_cores

    def makeBuilder(self, lib):
        cwd = os.getcwd()
        b = Builder(cwd, lib)
        b.num_cores = self.num_cores
        return b

    def thrift(self):
        b = self.makeBuilder(LibraryMetadata.thrift())

        b.build_options = """
CXXFLAGS='-g -O2'
CFLAGS='-g -O2'
--without-ruby
--without-erlang
--enable-gen-cpp
--with-boost={libs}/Boost""".format(libs=b.libs_fp())

        b.prepareAndBuild()
        self.__patch_thrift(b)

    def __patch_thrift(self, b):
        for f in ["thrift/include/thrift/protocol/TBinaryProtocol.h",
                  "thrift/include/thrift/protocol/TDenseProtocol.h"]:
            fnp = os.path.join(b.libPath, f)
            if not os.path.exists(fnp):
                raise Exception("can't patch " + fnp)

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
        path = os.path.join(os.getcwd(), "zi_lib")
        if os.path.exists(path):
            print "\nsubmodules already exists; skipping"

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

        bjamFlags = "-j{num}".format(num=self.num_cores)
        bjamFlags += " -sNO_BZIP2=1 -sZLIB_SOURCE=srcPath/ZLIB_VER"
        bjamFlags += " variant=release link=static threading=multi runtime-link=static"
        # bjamFlags += " toolset=gcc cxxflags=-std=gnu++0x"
        cmd = "./bjam " + bjamFlags + " install"
        print "building and installing (cmd)\n"
        os.system(cmd)
        b.chdir_home()
        print "done\n"

    def qt(self):
        b = self.makeBuilder(LibraryMetadata.qt())
        b.build_options = """ -release -opensource -no-glib -v
 -no-exceptions
 -no-fast -make libs -make tools
 -no-accessibility -no-qt3support -no-cups -no-qdbus -no-webkit
 -no-sql-sqlite -no-xmlpatterns -no-phonon -no-phonon-backend
 -no-svg -qt-zlib -qt-libtiff -qt-libpng -no-libmng
 -qt-libjpeg -no-openssl -no-nis -no-cups -no-iconv -no-freetype
 -no-multimedia -no-javascript-jit -no-script -no-scripttools"""

        b.prepareAndBuild()

    def hdf5(self):
        b = self.makeBuilder(LibraryMetadata.hdf5())
        b.build_options = "--enable-threadsafe --with-pthread=/usr/lib"
        b.prepareAndBuild()

    def omni(self):
        self.printTitle("omni")
        pwd = os.path.dirname(__file__)
        parent = os.path.abspath(os.path.join(pwd, '..'))
        os.chdir(parent)
        cmd = "make -j{0}".format(self.num_cores)
        os.system(cmd)

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
