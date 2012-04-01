#!/usr/bin/python

import os
import sys
from optparse import OptionParser

import bootstrap.sysutils
import bootstrap.runner
import bootstrap.detect_os
    
def menu():
    print "bootstrap.pl menu:"
    print "0 -- exit"
    print "1 -- Build All"
    print "2 -- Build boost"
    print "3 -- Build thrift"
    print "4 -- Build libjpeg"
    print "5 -- Build libpng"
    max_answer = 12
    
    while True:
        try:
            answer = int( raw_input( "Please make selection: " ) )
            if answer >= 0 and answer <= max_answer:
                runMenuEntry( answer )
                sys.exit()
        except KeyboardInterrupt:
            print ""
            sys.exit()
        except:
            pass
                    
def runMenuEntry(entry):
    if 0 == entry:
	return
    elif 1 == entry:
        bootstrap.runner.buildAll
    elif 2 == entry:
        bootstrap.runner.boost
    elif 3 == entry:
        bootstrap.runner.thrift
    elif 4 == entry:
        bootstrap.runner.libjpeg
    elif 5 == entry:
        bootstrap.runner.libpng
    elif 6 == entry:
        bootstrap.runner.zlib

def buildAll():
    bootstrap.runner.boost
    bootstrap.runner.thrift
    bootstrap.runner.libjpeg
    bootstrap.runner.libpng    
    
def setupParallelBuildOption(cmd_procs):
    numCores = bootstrap.sysutils.numberOfCores(cmd_procs)
    print "number of parallel builds (override with \"-j n\" switch to bootstrap.pl): ", numCores
                
def checkCmdLineArgs():
    parser = OptionParser()
    parser.add_option("-j", dest="cmd_procs", type="int", help="override num parallel builds")
    (options, args) = parser.parse_args()

    setupParallelBuildOption(options.cmd_procs)
        
    if len(args) == 0:
        menu()
    else:
        runMenuEntry( args[0] )

def doUbuntuAptGets():
    args = 'libxrender-dev libxext-dev freeglut3-dev g++ \
            libfreetype6-dev libxml2 libxml2-dev mesa-common-dev \
            libxt-dev libgl1-mesa-dev libglu1-mesa-dev libgl1-mesa-dri-dbg \
            libgl1-mesa-glx-dbg libncurses5-dev'

    print os.system("sudo apt-get -y install " + args)
    print "Done with the Ubuntu apt-gets! \n\n"

def omniClean():
    print "fix me"
    #os.system("cd " + omniPath make clean)`
    #omniServer()

bootstrap.detect_os.checkForMac()
checkCmdLineArgs()
