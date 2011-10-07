#pragma once

namespace om {
namespace utility {

class systemInformation{
public:
    static unsigned int get_num_cores();
    static unsigned int get_total_system_memory_megs();
};

} // namespace utility
} // namespace om