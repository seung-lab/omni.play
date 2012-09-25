import os
import sys

import fileutils
from string import Template

class Builder(object):

    def __init__(self, cwd, lib):
        self.basePath    = cwd
        self.baseFileName = lib.baseFileName
        self.libFolderName = lib.libFolderName
        self.uri = lib.uri
        self.build_options = ""
        self.num_cores = 1

        self.ext_fp      = self.basePath + '/external'
        self.buildPath   = self.basePath + '/external/builds'
        self.srcPath     = self.basePath + '/external/srcs'
        self.libPath     = self.basePath + '/external/libs'
        self.tarballPath = self.basePath + '/external/tarballs'
        self.scriptPath  = self.basePath + '/scripts'
        self.omniPath    = self.basePath

        self.globalMakeOptions = ""

        self.makeDirPaths

    def get_num_cores(self):
        return self._num_cores

    def set_num_cores(self, num):
        self._num_cores = num

        # TODO: make globalMakeOtions a hash of options...
        self.globalMakeOptions = " -j{num} ".format( num = num )

    num_cores = property(get_num_cores, set_num_cores)

    def get_build_options(self):
        return self._build_options

    def set_build_options(self, opts):
        self._build_options = " ".join(opts.strip().split("\n"))

    build_options = property(get_build_options, set_build_options)

    def src_fp(self):
        return os.path.join(self.srcPath, self.baseFileName)

    def lib_fp(self):
        return os.path.join(self.libPath, self.libFolderName)

    def libs_fp(self):
        return self.libPath

    def build_fp(self):
        return os.path.join(self.buildPath, self.baseFileName)

    def makeDirPaths(self):
        fileutils.ensure_dir(self.buildPath)
        fileutils.ensure_dir(self.srcPath)

    def setupBuildFolder(self):
        print "==> creating new build folder...",
        fileutils.ensure_dir(self.build_fp())
        print "done"

    def nukeSrcsFolder(self):
        print "==> removing old srcs folder...",
        fileutils.rm_f(self.src_fp())
        print "done"

    def nukeBuildFolder(self):
        print "==> removing old build folder...",
        fileutils.rm_f(self.build_fp())
        print "done"

    def nukeLibraryFolder(self):
        print "==> removing old library folder...",
        fileutils.rm_f(self.lib_fp())
        print "done"

    def untar(self):
        if fileutils.dir_exists(self.src_fp()):
            print "==> skipping untar"
            return

        print "==> untarring to external/srcs/...",
        fileutils.gunzip(self.tarball_fnp(), self.srcPath)
        print "done"

    # TODO: rename to "download", since we don't use wget
    def wget(self):
        fnp = self.tarball_fnp()

        if fileutils.file_exists(fnp):
            if fileutils.check_expected_wget_file_size(self.uri, fnp):
                print "==> skipping download"
                return

        size = fileutils.uri_size_bytes(self.uri)
        hrsize = fileutils.file_size_human_readable(size)
        print "==> downloading {0} to external/tarballs/...".format(hrsize),
        sys.stdout.flush()
        fileutils.ensure_dir(self.tarballPath)
        fileutils.wget(self.uri, fnp)
        print "done"

    def svn(self):
        fileutils.ensure_dir(self.src_fp())
        print "==> checking out from svn to {0}".format(self.src_fp()),
        sys.stdout.flush()
        fileutils.svn_co(self.uri, self.src_fp())
        print "done"

    def tarball_fnp(self):
        return os.path.join(self.tarballPath, self.baseFileName + ".tar.gz")

    def prepare(self):
        self.printTitle(self.baseFileName)
        self.nukeBuildFolder()
        self.nukeLibraryFolder()
        self.wget()
        self.untar()
        self.setupBuildFolder()

    def prepareSvn(self):
        self.printTitle(self.baseFileName)
        self.nukeBuildFolder()
        self.nukeLibraryFolder()
        self.svn()
        self.setupBuildFolder()

    def prepareNukeSrcsFolder(self):
        self.printTitle(self.baseFileName)
        self.nukeSrcsFolder()
        self.nukeBuildFolder()
        self.nukeLibraryFolder()
        self.wget()
        self.untar()
        self.setupBuildFolder()

    def build(self):
        self.chdir_build()

        self.configure()
        self.make()
        self.makeInstall()

        self.chdir_home()

    def buildInSourceFolder(self):
        self.chdir_src()

        self.configure()
        self.make()
        self.makeInstall()

        self.chdir_home()

    def chdir_src(self):
        os.chdir(os.path.join(self.srcPath, self.baseFileName))

    def chdir_build(self):
        os.chdir(os.path.join(self.buildPath, self.baseFileName))

    def chdir_home(self):
        os.chdir(self.basePath)

    def configure(self):
        configure = os.path.join(self.srcPath, self.baseFileName, "configure")
        cmd = "chmod +x {c}; ".format(c = configure)

        if "qt" == self.libFolderName:
            cmd += 'echo "yes" | '

        cmd += "{c} --prefix={p} {bopts}".format(c = configure,
                                                 p = self.lib_fp(),
                                                 bopts = self.build_options)

        print "==> running configure..."
        print "cmd:", cmd
        # TODO: check return values die if something went wrong...
        print os.system(cmd)
        print "done with configure"

    def make(self):
        cmd = "make {gmo}".format(gmo = self.globalMakeOptions)
        print "==> running make..."
        print cmd
        # TODO: check return values die if something went wrong...
        print os.system(cmd)
        print "done with make"

    def makeInstall(self):
        cmd = "make install"
        print "==> running make install..."
        print cmd
        # TODO: check return values die if something went wrong...
        print os.system(cmd)
        print "done with make install"

    def prepareAndBuild(self):
        self.prepare()
        self.build()

    def prepareNukeSrcsAndBuild(self):
        self.prepareNukeSrcsFolder()
        self.build()

    def printTitle(self, title):
        self.printLine()
        print title + ":\n"

    def printLine(self):
        print "\n**********************************************\n"
