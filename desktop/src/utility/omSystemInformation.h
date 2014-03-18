#pragma once
#include "precomp.h"

class OmSystemInformation {
 public:
  static unsigned int get_num_cores();
  static unsigned int get_total_system_memory_megs();
};
