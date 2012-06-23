#!/usr/bin/python

import os
import sys
import traceback
from optparse import OptionParser

from bootstrap.sysutils import sysutils
from bootstrap.runner import runner
from bootstrap.detect_os import detect_os
from bootstrap.sys_mac import sys_mac

class bootstrap:

    def show_menu(self, options):
        max_answer = len(options) - 1

        print "bootstrap.py menu:"

        for idx, val in enumerate(options):
            print "{i} -- {val}".format(i = idx,
                                        val = val)

        while True:
            answer = None
            try:
                s = raw_input( "Please make selection: " )
                answer = int( s )
            except KeyboardInterrupt, SystemExit:
                print ""
                sys.exit(0)
            except:
                print "could not parse \"" + s + "\""
                pass

            if answer >= 0 and answer <= max_answer:
                self.runMenuEntry( answer )
                sys.exit(0)

    def menu(self):
        options = [ "exit",
                    "Build All",
                    "Build boost",
                    "Build thrift",
                    "Build libjpeg",
                    "Build libpng",
                    "Build zlib",
                    "Build libevent",
                    "Build qt",
                    "Build hdf5",
                    "Init Submodules",
                    "Install Ubuntu dev packages"]

        self.show_menu(options)

    def runMenuEntry(self, entry):
        r = runner(self.numCores)
        try:
            if 0 == entry:
                sys.exit(0)
            elif 1 == entry:
                self.buildAll()
            elif 2 == entry:
                r.boost()
            elif 3 == entry:
                r.thrift()
            elif 4 == entry:
                r.libjpeg()
            elif 5 == entry:
                r.libpng()
            elif 6 == entry:
                r.zlib()
            elif 7 == entry:
            	r.libevent()
            elif 8 == entry:
            	r.qt()
            elif 9 == entry:
            	r.hdf5()
            elif 10 == entry:
            	r.submodule()
            elif 11 == entry:
                self.doUbuntuAptGets()
            else:
                print "unknown option: ", entry
                sys.exit(1)
        except SystemExit, e:
            sys.exit(e)
        except Exception, e:
            print str(e)
            print "*******************************"
            traceback.print_exc()

    def buildAll(self):
        r = runner(self.numCores)
        self.doUbuntuAptGets()
        r.submodule()
        r.thrift()
        r.libjpeg()
        r.libpng()
        r.libevent()
        r.hdf5()
        r.boost()
        r.qt()

    def parallelCompilation(self, cmd_procs):
        self.numCores = sysutils.numberOfCores(cmd_procs)
        print "number of parallel builds (override with \"-j n\" switch to bootstrap.pl): ", self.numCores

    def checkCmdLineArgs(self):
        parser = OptionParser()
        parser.add_option("-j", dest="cmd_procs", type="int", help="override num parallel builds")
        (options, args) = parser.parse_args()

        self.parallelCompilation(options.cmd_procs)

        if len(args) == 0:
            self.menu()
        else:
            self.runMenuEntry( int(args[0]) )

    def doUbuntuAptGets(self):
        args = 'libxrender-dev libxext-dev freeglut3-dev g++ \
libfreetype6-dev libxml2 libxml2-dev mesa-common-dev \
libxt-dev libgl1-mesa-dev libglu1-mesa-dev libgl1-mesa-dri-dbg \
libgl1-mesa-glx-dbg libncurses5-dev nasm libevent-dev'

        cmd = "sudo apt-get -y install " + args
        print "about to run: " + cmd
        print os.system(cmd)
        print "Done with the Ubuntu apt-gets! \n\n"

    def omniClean(self):
        print "fix me"
        #os.system("cd " + omniPath make clean)`
        #omniServer()

def main(argv=None):
    if argv is None:
        argv = sys.argv
    # etc., replacing sys.argv with argv in the getopt() call.

    sys_mac.checkForMac()

    b = bootstrap()
    b.checkCmdLineArgs()

if __name__ == "__main__":
    main()
