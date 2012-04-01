import os
import fileutils
from string import Template

class Builder:

    def __init__(self, cwd, baseFileName, libFolderName, uri, buildOptions = ""):
        self.basePath    = cwd
        self.baseFileName = baseFileName
        self.libFolderName = libFolderName
        self.uri = uri
        self.buildOptions = buildOptions

        self.buildPath   = self.basePath + '/external/builds'
        self.srcPath     = self.basePath + '/external/srcs'
        self.libPath     = self.basePath + '/external/libs'
        self.tarballPath = self.basePath + '/external/tarballs'
        self.scriptPath  = self.basePath + '/scripts'
        self.omniPath    = self.basePath

        self.globalMakeOptions = ""

        self.makeDirPaths

    def numCores(self, num):
        self.numCores = num
        
        # TODO: make globalMakeOtions a hash of options...
        self.globalMakeOptions = " -j{num} ".format( num = num )

    def src_fp(self):
        return os.path.join(self.srcPath, self.baseFileName)

    def lib_fp(self):
        return os.path.join(self.libPath, self.libFolderName)

    def build_fp(self):
        return os.path.join(self.buildPath, self.baseFileName)

    def makeDirPaths(self):
        fileutils.ensure_dir(self.buildPath)
        fileutils.ensure_dir(self.srcPath)

    def setupBuildFolder(self):
        print "==> creating new build folder..."
        fileutils.ensure_dir(self.build_fp())
        print "done\n"

    def nukeSrcsFolder(self):
        print "==> removing old srcs folder..."
        fileutils.rm_f(self.src_fp())
        print "done\n"

    def nukeBuildFolder(self):
        print "==> removing old build folder..."
        fileutils.rm_f(self.build_fp())
        print "done\n"

    def nukeLibraryFolder(self):
        print "==> removing old library folder..."
        fileutils.rm_f(self.lib_fp())
        print "done\n"

    def untar(self):
        if fileutils.dir_exists(self.src_fp()):
            print "==> skipping untar\n"
            return
        
        print "==> untarring to external/srcs/..."
        fileutils.gunzip(self.tarball_fnp(), self.srcPath)
        print "done\n"

    def wget(self):
        fnp = self.tarball_fnp()
        
        if fileutils.file_exists(fnp):
            print "==> skipping wget\n"
            return
        
        print "==> wgetting to external/tarballs/..."
        fileutils.ensure_dir(self.tarballPath)
        fileutils.wget(self.uri, fnp)
        print "done\n"

    def tarball_fnp(self):
        return os.path.join(self.tarballPath, self.baseFileName + ".tar.gz")
    
    def prepare(self):
        self.printTitle(self.baseFileName)
        self.nukeBuildFolder()
        self.nukeLibraryFolder()
        self.wget()
        self.untar()
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
        cmd += "{c} --prefix={p} {bopts}".format(c = configure,
                                                 p = self.lib_fp(),
                                                 bopts = self.buildOptions)
        if "Qt" == self.libFolderName:
            cmd = 'echo "yes" | '.cmd
            
        print "==> running configure..."
        print cmd
        # TODO: check return values die if something went wrong...
        print os.system(cmd)
        print "done with configure\n\n"

    def make(self):
        cmd = "make {gmo}".format(gmo = self.globalMakeOptions)
        print "==> running make..."
        print cmd
        # TODO: check return values die if something went wrong...
        print os.system(cmd)
        print "done with make\n\n"

    def makeInstall(self):
        cmd = "make install"
        print "==> running make install..."
        print cmd
        # TODO: check return values die if something went wrong...
        print os.system(cmd)
        print "done with make install\n"

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
