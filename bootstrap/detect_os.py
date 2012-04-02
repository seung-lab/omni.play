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
import re

class detect_os:

    @staticmethod
    def isMac():
        return 'Darwin' == platform.system()
    
    @staticmethod
    def isLinux():
        return "posix" == os.name and "Linux" == platform.system()

    @staticmethod
    def isWindowsNative():
        return "nt" == os.name

    @staticmethod
    def isWindowsCygwin():
        return "posix" == os.name and None != re.match("CYGWIN.*", platform.system())
