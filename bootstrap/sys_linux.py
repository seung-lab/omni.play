from cmd import cmd

class sys_linux:
    @staticmethod
    def numCoresLinux():
        return cmd.regex_cmd_count("cat /proc/cpuinfo", "processor")
