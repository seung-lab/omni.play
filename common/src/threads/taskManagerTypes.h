#pragma once

#include "threads/taskManagerContainerDeque.hpp"

namespace om {
namespace threads {

class taskManagerContainerMipSorted;
template <class> class taskManager;

typedef taskManager<taskManagerContainerDeque> threadPool;

}
}
