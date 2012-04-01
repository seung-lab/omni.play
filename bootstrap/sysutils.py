import os
import subprocess
import re
import fileutils
import detect_os 

def numberOfCores(override):
    numCores = 2

    if None == override:     
        if detect_os.isLinux():
            numCores = numCoresLinux()

        elif detect_os.isMac():
            numCores = numCoresMac()
    else:
        numCores = override
    
    return numCores

def numCoresLinux():
    return regex_cmd_count("cat /proc/cpuinfo", "processor")

def numCoresMac():
    m = regex_cmd_int("/usr/sbin/system_profiler SPHardwareDataType",
                      ".*Total Number Of Cores: (\d).*")
    if None == m:
        print "could not find num cores"
        sys.exit()
    return m

def regex_cmd_int(cmd, regex):
    match = regex_cmd_search(cmd, regex)
    if match:
        return int( match.group(1) )
    else:
        return None

def regex_cmd_search(cmd, regex):
    proc = subprocess.Popen(cmd, shell=True,
                            stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    output = proc.communicate()[0]
    return re.search(regex, output)

def regex_cmd_count(cmd, regex):
    proc = subprocess.Popen(cmd, shell=True,
                            stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    output = proc.communicate()[0]
    return len(re.findall(regex, output))
