#pragma once

#include "threads/taskManagerContainerDeque.hpp"

class taskManagerContainerMipSorted;
template <class> class taskManager;

typedef taskManager<taskManagerContainerDeque> threadPool;

