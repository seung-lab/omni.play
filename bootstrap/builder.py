import subprocess
import fileutils

def __init__(self):
    self.buildPath = ""
    self.srcPath = ""
    self.baseFileName = ""
    self.libFolderName = ""
    self.uri = ""
    self.buildOptions = ""
    self.basePath = ""

def makeDirPaths():
    fileutils.ensure_dir(self.buildPath)
    fileutils.ensure_dir(self.srcPath)

def genOmniScript():
    script = "cd basePath/omni.server"
    print "not yet converted"
    pass
"""
    if(@_ == 1):
	debugMode = _[0]
	if(debugMode){
	    print "building omni.server in debug mode\n"
	    script += "rm -f release"
	 else:
	    print "building omni.server in release mode\n"
	    script += "touch release"
	
     else:
	print "building omni.server with current make options\n"
    

    script += "qmake omni.server.pro"

    if(@_ == 1):
	print "will make clean\n"
	script += "make clean"
    

    script += "make globalMakeOptions"

#     if(isMac()):
#         nigOutPath = "basePath/omni/bin/omni.app/Contents/Resources/"
# 	script += " cp -r ../external/QT_VER/src/gui/mac/qt_menu.nib nigOutPath\n"
#     

    open (SCRIPT, ">", omniScriptFile) or die !
    print SCRIPT script
    close SCRIPT
    `chmod +x omniScriptFile`
"""

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

def src_fp():
    self.srcPath + "/" + self.baseFileName
def lib_fp():
    self.libPath + "/" + self.libFolderName
def build_fp():
    self.buildPath + "/" + self.baseFileName

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

def configure(baseFileName, libFolderName, buildOptions):
    cmd = "chmod +x self.srcPath/baseFileName/configureself.srcPath/baseFileName/configure --prefix=libPath/libFolderName buildOptions"

    if( "Qt" eq libFolderName ){
	cmd = 'echo "yes" | '.cmd
    
    print "==> running configure..."
    print "(cmd)\n"
    # TODO: check return values die if something went wrong...
    print `(cmd)`
    print "done with configure\n\n"

def make():
    cmd = "make globalMakeOptions"
    print "==> running make..."
    print "(cmd)\n"
    # TODO: check return values die if something went wrong...
    print `(cmd)`
    print "done with make\n\n"

def makeInstall():
    cmd = "make install"
    print "==> running make install..."
    print "(cmd)\n"
    # TODO: check return values die if something went wrong...
    print `(cmd)`
    print "done with make install\n"

def prepareAndBuild:
    prepare
    build

def prepareNukeSrcsAndBuild():
    prepareNukeSrcsFolder
    build
