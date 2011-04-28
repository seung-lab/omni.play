#ifndef OM_TASK_MANAGER_TYPES_HPP
#define OM_TASK_MANAGER_TYPES_HPP

#include "threads/omTaskManagerContainerDeque.hpp"

template <class> class OmTaskManager;

typedef OmTaskManager<OmTaskManagerContainerDeque> OmThreadPool;

#endif
