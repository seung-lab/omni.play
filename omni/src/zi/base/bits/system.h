#ifndef _ZI_BASE_BITS_SYSTEM_H_
#define _ZI_BASE_BITS_SYSTEM_H_

# if (defined(WIN32)   || defined(__WIN32__)            \
      || defined(__WIN32) || defined(_WIN32)) &&        \
  !(defined(__CYGWIN32__) || defined(__CYGWIN__))
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#else
#  include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined(_WIN32)
#  ifndef _SC_NPROCESSORS_ONLN
#    define _SC_NPROCESSORS_ONLN 1
#    define _SC_NPROCESSORS_CONF 1

inline long int sysconf(int i) {

  SYSTEM_INFO info;
  GetSystemInfo(&info);

  switch (i) {
  _SC_NPROCESSORS_ONLN:
    return info.dwNumberOfProcessors;
    break;
  default:
    return 0;
  }
  return 0;
}

#  endif
#else
#  ifndef _SC_NPROCESSORS_ONLN
#    define _SC_NPROCESSORS_ONLN 0
#    define _SC_NPROCESSORS_CONF 0
#    define sysconf(igore) 0
#  endif
#endif

#endif
