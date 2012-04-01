import fileutils

class Builder:

    def __init__(cwd, baseFileName, libFolderName, uri):
        self.basePath    = cwd
        self.baseFileName = baseFileName
        self.libFolderName = libFolderName
        self.uri = uri

        self.buildPath   = self.basePath + '/external/builds'
        self.srcPath     = self.basePath + '/external/srcs'
        self.libPath     = self.basePath + '/external/libs'
        self.tarballPath = self.basePath + '/external/tarballs'
        self.scriptPath  = self.basePath + '/scripts'
        self.omniPath    = self.basePath

        self.buildOptions = ""

        self.globalMakeOptions = ""

    def src_fp():
        self.srcPath + "/" + self.baseFileName

    def lib_fp():
        self.libPath + "/" + self.libFolderName

    def build_fp():
        self.buildPath + "/" + self.baseFileName

    def makeDirPaths():
        fileutils.ensure_dir(self.buildPath)
        fileutils.ensure_dir(self.srcPath)

    def setupBuildFolder(baseFileName):
        print "==> creating new build folder..."
        filesutils.ensure_dir(build_fp)
        print "done\n"

    def nukeSrcsFolder(baseFileName):
        print "==> removing old srcs folder..."
        fileutils.rm_f(src_fp)
        print "done\n"

    def nukeBuildFolder(baseFileName):
        print "==> removing old build folder..."
        fileutils.rm_f(build_fp)
        print "done\n"

    def nukeLibraryFolder():
        print "==> removing old library folder..."
        fileutils.rm_f(lib_fp)
        print "done\n"

    def untar():
        if fileutils.dir_exists(src_fp):
            print "==> skipping untar\n"
            return
        
        print "==> untarring to external/srcs/..."
        fileutils.gunzip(tarball_fnp, self.srcPath)
        print "done\n"

    def wget():
        fnp = tarball_fnp
        
        if fileutils.file_exists(fnp):
            print "==> skipping wget\n"
            return
        
        print "==> wgetting to external/tarballs/..."
        fileutils.ensure_dir(self.tarballPath)
        fileutils.wget(uri, fnp)
        print "done\n"

    def tarball_fnp():
        self.tarballPath + "/" + self.baseFileName + ".tar.gz"
    
    def prepare():
        printTitle
        nukeBuildFolder
        nukeLibraryFolder
        wget
        untar
        setupBuildFolder

    def prepareNukeSrcsFolder():
        printTitle
        nukeSrcsFolder
        nukeBuildFolder
        nukeLibraryFolder
        wget
        untar
        setupBuildFolder
    
    def build():
        chdir_src
        
        configure
        make
        makeInstall
        
        chdir_home
    
    def chdir_src():
        os.chdir(self.srcPath + "/" + self.baseFileName)

    def chdir_home():
        os.chdir(self.basePath)

    def buildInSourceFolder():
        chdir_src
        
        configure
        make
        makeInstall
        
        chdir_home

    def configure():
        cmd = "chmod +x {c}; ".format(c = configure)
        cmd += "{c} --prefix={p} {bopts}".format(c = configure,
                                                 p = lib_fp,
                                                 bopts = buildOptions)
        if "Qt" == self.libFolderName:
            cmd = 'echo "yes" | '.cmd
            
        print "==> running configure..."
        print "(cmd)\n"
        # TODO: check return values die if something went wrong...
        print os.system(cmd)
        print "done with configure\n\n"

    def make():
        cmd = "make globalMakeOptions"
        print "==> running make..."
        print "(cmd)\n"
        # TODO: check return values die if something went wrong...
        print os.system(cmd)
        print "done with make\n\n"

    def makeInstall():
        cmd = "make install"
        print "==> running make install..."
        print "(cmd)\n"
        # TODO: check return values die if something went wrong...
        print os.system(cmd)
        print "done with make install\n"

    def prepareAndBuild:
        prepare
        build

    def prepareNukeSrcsAndBuild():
        prepareNukeSrcsFolder
        build
