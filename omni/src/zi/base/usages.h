/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef COMMON_BASE_USAGES_H_
#define COMMON_BASE_USAGES_H_

#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>

int getMemUsage(std::string &_ret) {

  _ret = "";

  int fd_in = open("/proc/self/status", O_RDONLY);
  if (fd_in < 0) return -1;

  char buffer[4001];
  int n = read(fd_in, buffer, 4000);
  if (n < 0) return -1;
  buffer[n] = '\0';

  if (close(fd_in) < 0) return -1;

  char *hit = strstr(buffer, "VmPeak");
  if (!hit) return -1;

  long vmpeak, vmsize, vmlck, vmhwm, vmrss, vmdata, vmstk, vmexe, vmlib, vmpte;
  sscanf(hit, "VmPeak: %ld kB\nVmSize: %ld kB\n"
         "VmLck: %ld kB\nVmHWM: %ld kB\nVmRSS: %ld kB\nVmData: %ld kB\n"
         "VmStk: %ld kB\nVmExe: %ld kB\nVmLib: %ld kB\nVmPTE: %ld kB\n",
         &vmpeak, &vmsize, &vmlck, &vmhwm, &vmrss, &vmdata, &vmstk, &vmexe,
         &vmlib, &vmpte);

  char retBuffer[4000];

  sprintf(retBuffer, "Peak: %ld MB, Size: %ld MB, "
          "Lck: %ld MB, HWM: %ld MB, RSS: %ld MB, Data: %ld MB, "
          "Stk: %ld MB, Exe: %ld MB, Lib: %ld MB, PTE: %ld MB, ",
          vmpeak / 1024, vmsize / 1024,
          vmlck / 1024, vmhwm / 1024,
          vmrss / 1024, vmdata / 1024,
          vmstk / 1024, vmexe / 1024,
          vmlib / 1024, vmpte / 1024);

  _ret = std::string(retBuffer);
  return 0;
}

class ZUSAGES {
public:
  static std::string getMemUsageString() {
    std::string usages;
    if (getMemUsage(usages) < 0) {
      return "ERROR GETING MEM USAGES";
    }
    return usages;
  }
private:
  ZUSAGES() {};
};

#endif
