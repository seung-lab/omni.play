#include "precomp.h"
#include "utility/systemInformation.h"

namespace om {
namespace utility {

unsigned int systemInformation::get_num_cores() {
  return zi::system::cpu_count;
}

unsigned int systemInformation::get_total_system_memory_megs() {
  return zi::system::memory::total_mb();
}

}  // namespace utility
}  // namespace om