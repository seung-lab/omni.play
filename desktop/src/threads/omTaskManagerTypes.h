#pragma once
#include "precomp.h"

#include "threads/omTaskManagerContainerDeque.hpp"

class OmTaskManagerContainerMipSorted;
template <class>
class OmTaskManager;

typedef OmTaskManager<OmTaskManagerContainerDeque> OmThreadPool;
