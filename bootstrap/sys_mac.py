import platform
import subprocess
import re
import sys

from detect_os import detect_os
from cmd import cmd

class sys_mac:
    def isMac(self):
        return detect_os.isMac()

    def getMacOSXversionNum(self):
        cmd = "/usr/sbin/system_profiler SPSoftwareDataType"
        proc = subprocess.Popen(cmd, shell=True,
                                stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        output = proc.communicate()[0]
        match = re.search(r".*System Version: Mac OS X 10\.(\d)\..*", output)
        if match:
            return int( match.group(1) )
        else:
            print "could not find version number"
            sys.exit()

    def isMacLeopard(self):
        return self.isMac() and 5 == self.getMacOSXversionNum()

    def isMacSnowLeopard(self):
        return self.isMac() and 6 == self.getMacOSXversionNum()

    def isMacLion(self):
        return self.isMac() and 7 == self.getMacOSXversionNum()

    def isMacMountainLion(self):
        return self.isMac() and 8 == self.getMacOSXversionNum()

    @staticmethod
    def checkForMac():
        sm = sys_mac()
        if sm.isMac():
            a = "Mac OS X version is: {0}".format(sm.getMacOSXversionNum())

            if sm.isMacLeopard():
                a += " (Leopard)"
            elif sm.isMacSnowLeopard():
                a += " (Snow Leopard)"
            elif sm.isMacLion():
                a += " (Lion)"
            elif sm.isMacMountainLion():
                a += " (Mountain Lion)"
            else:
                a += " (unknown)"
            print a

    @staticmethod
    def numCoresMac():
        m = cmd.regex_cmd_int("/usr/sbin/system_profiler SPHardwareDataType  | grep 'Cores'",
                              ".*Total Number of Cores.*(\d).*")
        if None == m:
            print "could not find num cores"
            sys.exit()
        return m
