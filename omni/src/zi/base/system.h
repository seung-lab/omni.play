#ifndef _ZI_BASE_SYSTEM_H_
#define _ZI_BASE_SYSTEM_H_

#include "bits/system.h"

namespace zi { namespace System {

inline int getCPUCount() {
  return sysconf(_SC_NPROCESSORS_CONF);
}

} }

#endif
