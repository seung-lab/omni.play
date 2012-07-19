import subprocess
import re
import sys

class cmd:
    @staticmethod
    def regex_cmd_int(c, regex):
        match = cmd.regex_cmd_search(c, regex)
        if match:
            return int( match.group(1) )
        else:
            return None

    @staticmethod
    def regex_cmd_search(cmd, regex):
        proc = subprocess.Popen(cmd, shell=True,
                                stdin=subprocess.PIPE,
                                stdout=subprocess.PIPE)
        output, stderr = proc.communicate()
        output = output.strip()
        if stderr:
            print "regex: error:", stderr
            print "\tregex was:", cmd, regex
            sys.exit(1)
        return re.search(regex, output)

    @staticmethod
    def regex_cmd_count(cmd, regex):
        proc = subprocess.Popen(cmd, shell=True,
                                stdin=subprocess.PIPE, 
                                stdout=subprocess.PIPE)
        output, stderr = proc.communicate()
        if stderr:
            print "regex: error:", stderr
            print "\tregex was:", cmd, regex
            sys.exit(1)
        return len(re.findall(regex, output))
