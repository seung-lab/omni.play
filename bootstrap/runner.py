import fileutils
import os

from string import Template
from library import LibraryMetadata
from builder import Builder
from sysutils import sysutils

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
--with-boost={libs}/boost""".format(libs=b.libs_fp())

        b.prepareAndBuild()

    def libjpeg(self):
        b = self.makeBuilder(LibraryMetadata.jpeg())

        if sysutils.isMac():
            print "libjpeg not building on mac"
            return

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
        b = self.makeBuilder(LibraryMetadata.boost())
        b.prepareNukeSrcsFolder()

        bz = self.makeBuilder(LibraryMetadata.zlib())
        bz.wget()
        bz.untar()

        b.chdir_src()
        cmd = "./bootstrap.sh --prefix=" + b.lib_fp()
        cmd += " --with-libraries=date_time,filesystem,iostreams,math,program_options,random,regex,serialization,signals,system,test,thread,log"

        print "configuring (" + cmd + ")\n"

        os.system(cmd)
        print "done\n"

        bjamFlags = "-j{num}".format(num=self.num_cores)
        bjamFlags += " -sNO_BZIP2=1 -sZLIB_SOURCE=srcPath/ZLIB_VER"
        bjamFlags += " variant=release link=static threading=multi runtime-link=static"
        bjamFlags += " toolset=gcc cxxflags=-std=c++11"
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

    def netlib(self):
        b = self.makeBuilder(LibraryMetadata.netlib())
        b.prepareNukeSrcsFolder()
        self.__patch_netlib(b);
        boost = self.makeBuilder(LibraryMetadata.boost())
        # see http://stackoverflow.com/a/17049807 for build_options
        b.build_options = """
-DBoost_NO_BOOST_CMAKE=TRUE
-DBoost_NO_SYSTEM_PATHS=TRUE
-DBOOST_ROOT:PATHNAME={boost}
-DBoost_LIBRARY_DIRS:FILEPATH={boost}/lib
""".format(boost=boost.lib_fp())
        b.buildCmake()

    def __patch_netlib(self, b):
        ext_fp = b.ext_fp
        srcPath = b.srcPath
        patch_fnp = os.path.join(ext_fp, "patches/CMakeLists.txt.patch")

        cmd = "/usr/bin/patch -d {srcPath} -p0 -i {fnp}".format(srcPath=srcPath,
                                                               fnp=patch_fnp)
        print "patching using cmd: ", cmd
        os.system(cmd)

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
