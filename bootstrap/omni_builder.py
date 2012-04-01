        self.omniMacExec = '/bin/omni.server.app/Contents/MacOS/omni.server'
        self.omniExecPathMac = self.basePath + self.omniMacExec
        self.omniScriptFile = self.scriptPath + '/buildomni.sh'
        self.omniScriptOptions = ""


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
