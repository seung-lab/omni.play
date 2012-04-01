"""
table from
ttp://stackoverflow.com/questions/1387222/reliably-detect-windows-in-python

OS/build     | os.name | platform.system() 
-------------+---------+-----------------------
Win32 native | nt      | Windows
Win32 cygwin | posix   | CYGWIN_NT-5.1*
Win64 native | nt      | Windows
Win64 cygwin | posix   | CYGWIN_NT-6.1-WOW64*
Linux        | posix   | Linux
"""

import os
import platform
import subprocess
import re
import sys

def isMac():
    return 'Darwin' == platform.system()

def getMacOSXversionNum():
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

def isMacLeopard():
    return isMac() and 5 == getMacOSXversionNum()

def isMacSnowLeopard():
    return isMac() and 6 == getMacOSXversionNum()

def checkForMac():
    if isMac():
        a = "Mac OS X version is: " + getMacOSXversionNum()

        if isMacLeopard():
            a += " (Leopard)"
        elif isMacSnowLeopard():
            a += " (Snow Leopard)"
        else:
            a += " (unknown)"
        print a

def isLinux():
    return "posix" == os.name and "Linux" == platform.system()

def isWindowsNative():
    return "nt" == os.name

def isWindowsCygwin():
    return "posix" == os.name and None != re.match("CYGWIN.*", platform.system())
