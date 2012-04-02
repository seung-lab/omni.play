import subprocess
import re

class cmd:
    @staticmethod
    def regex_cmd_int(cmd, regex):
        match = cmd.regex_cmd_search(cmd, regex)
        if match:
            return int( match.group(1) )
        else:
            return None

    @staticmethod
    def regex_cmd_search(cmd, regex):
        proc = subprocess.Popen(cmd, shell=True,
                                stdin=subprocess.PIPE,
                                stdout=subprocess.PIPE)
        output = proc.communicate()[0]
        return re.search(regex, output)

    @staticmethod
    def regex_cmd_count(cmd, regex):
        proc = subprocess.Popen(cmd, shell=True,
                                stdin=subprocess.PIPE, 
                                stdout=subprocess.PIPE)
        output = proc.communicate()[0]
        return len(re.findall(regex, output))
