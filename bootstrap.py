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
    def __init__(self):
        (options, args) = self.__parseArgs()
        self.numCores = self.__cores(options.cmd_procs)
        self.r = runner(self.numCores)
        self.options = self.__setupOptions()

        if len(args) == 0:
            self.__menu()
        else:
            self.__run_idx(int(args[0]))

    def __setupOptions(self):
        # --the ordering of the options matters:
        #   We assume first 4 options (exit, all, init, and apt-get),
        #   and the final "Build Omni" option, don't change.
        # --This ordering let's us "Build All" easily since
        #   1) we can skip the exit, init submodules, and apt-get steps
        #   2) Build Omni is the last thing done
        # --When adding a library, just add it between apt-get and Build Omni
        #   (and resort these lines, if desired, to keep in alphabetical
        #    order)
        return [["exit", self.__exit],
                ["Build All", self.__buildAll ],
                ["Init Submodules", self.r.submodule ],
                ["apt-get installs", self.__doUbuntuAptGets],
                ["Build boost", self.r.boost ],
                ["Build hdf5", self.r.hdf5 ],
                ["Build libevent", self.r.libevent ],
                ["Build libjpeg", self.r.libjpeg ],
                ["Build libpng", self.r.libpng ],
                ["Build qt", self.r.qt ],
                ["Build thrift", self.r.thrift],
                ["Build zlib", self.r.zlib ],
                ["Build Omni", self.r.omni] ]

    def __menu(self):
        print "bootstrap.py menu:"
        for idx, val in enumerate(self.options):
            print "{i} -- {v}".format(i = idx,
                                      v = val[0])
        while True:
            idx = None
            try:
                s = raw_input( "Please make selection: " )
                idx = int( s )
            except KeyboardInterrupt, SystemExit:
                print ""
                sys.exit(0)
            except:
                print "could not parse \"" + s + "\""
                pass

            if self.__run_idx(idx):
                sys.exit(0)

    def __run_idx(self, idx):
        max_idx = len(self.options) - 1
        if idx >= 0 and idx <= max_idx:
            try:
                self.options[idx][1]()
                return True
            except SystemExit, e:
                sys.exit(e)
            except Exception, e:
                print str(e)
                print "*******************************"
                traceback.print_exc()
        return False

    def __exit(self):
        sys.exit(0)

    def __buildAll(self):
        #skip first 4 options
        for idx, e in enumerate(self.options[4:]):
            e[1]()

    def __parseArgs(self):
        parser = OptionParser()
        parser.add_option("-j", dest="cmd_procs", type="int",
                          help="override num parallel builds")
        return parser.parse_args()

    def __cores(self, cmd_procs):
        numCores = sysutils.numberOfCores(cmd_procs)
        print "number of parallel builds ",
        print "(override with \"-j n\" switch to bootstrap.pl): ",
        print numCores
        return numCores

    def __doUbuntuAptGets(self):
        args = 'libxrender-dev libxext-dev freeglut3-dev g++ \
libfreetype6-dev libxml2 libxml2-dev mesa-common-dev \
libxt-dev libgl1-mesa-dev libglu1-mesa-dev libgl1-mesa-dri-dbg \
libgl1-mesa-glx-dbg libncurses5-dev nasm libevent-dev libssl-dev \
libcurl4-openssl-dev pkg-config'

        cmd = "sudo apt-get -y install " + args
        print "about to run: " + cmd
        print os.system(cmd)
        print "Done with the Ubuntu apt-gets! \n\n"

def main(argv=None):
    if argv is None:
        argv = sys.argv
    # etc., replacing sys.argv with argv in the getopt() call.

    sys_mac.checkForMac()

    b = bootstrap()

if __name__ == "__main__":
    main()
