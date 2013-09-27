#pragma once

#include "threads/taskManagerContainerDeque.hpp"

namespace om {
namespace thread {

class TaskManagerContainerMipSorted;
template <class>
class TaskManager;

typedef TaskManager<TaskManagerContainerDeque> ThreadPool;
}
}
