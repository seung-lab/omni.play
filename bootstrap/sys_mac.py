import platform
import subprocess
import re
from detect_os import detect_os

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
        return isMac() and 5 == getMacOSXversionNum()

    def isMacSnowLeopard(self):
        return isMac() and 6 == getMacOSXversionNum()

    @staticmethod
    def checkForMac():
        sm = sys_mac()
        if sm.isMac():
            a = "Mac OS X version is: " + getMacOSXversionNum()

            if sm.isMacLeopard():
                a += " (Leopard)"
            elif sm.isMacSnowLeopard():
                a += " (Snow Leopard)"
            else:
                a += " (unknown)"
            print a

    def numCoresMac(self):
        m = regex_cmd_int("/usr/sbin/system_profiler SPHardwareDataType",
                          ".*Total Number Of Cores: (\d).*")
        if None == m:
            print "could not find num cores"
            sys.exit()
        return m
