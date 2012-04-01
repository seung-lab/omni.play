#!/usr/bin/python

import os
import sys
import traceback
from optparse import OptionParser

import bootstrap.sysutils
import bootstrap.runner
import bootstrap.detect_os
    
def menu():
    options = [ "exit",
                "Build All",
                "Build boost",
                "Build thrift",
                "Build libjpeg",
                "Build libpng",
                "Build zlib"]

    max_answer = len(options) - 1

    print "bootstrap.pl menu:"

    for idx, val in enumerate(options):
        print "{i} -- {val}".format(i = idx,
                                    val = val)

    while True:
        s = raw_input( "Please make selection: " )
        answer = None
        try:
            answer = int( s )
                
        except:
            print "could not parse \"" + s + "\""
            pass

        print "'" + str(answer) + "'"
        if answer >= 0 and answer <= max_answer:
            runMenuEntry( answer )
            sys.exit()
                    
def runMenuEntry(entry):
    try:
        if 0 == entry:
            sys.exit()
        elif 1 == entry:
            bootstrap.runner.buildAll()
        elif 2 == entry:
            bootstrap.runner.boost()
        elif 3 == entry:
            bootstrap.runner.thrift()
        elif 4 == entry:
            bootstrap.runner.libjpeg()
        elif 5 == entry:
            bootstrap.runner.libpng()
        elif 6 == entry:
            bootstrap.runner.zlib()
        else:
            print "unknown option: ", entry
    except SystemExit, e:
        sys.exit(e)
    except Exception, e:
        print str(e)
        print "*******************************"
        traceback.print_exc()

def buildAll():
    bootstrap.runner.boost()
    bootstrap.runner.thrift()
    bootstrap.runner.libjpeg()
    bootstrap.runner.libpng() 
    
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
        runMenuEntry( int(args[0]) )

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
