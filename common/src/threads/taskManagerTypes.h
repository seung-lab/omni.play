#pragma once

#include "threads/taskManagerContainerDeque.hpp"
#include "threads/taskManager.hpp"

namespace om {
namespace thread {
typedef TaskManager<TaskManagerContainerDeque> ThreadPool;
extern template class TaskManager<TaskManagerContainerDeque>;
}
}
