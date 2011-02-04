#ifndef OM_VIEW2D_MANAGER_HPP
#define OM_VIEW2D_MANAGER_HPP

#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "view2d/omView2dManagerImpl.hpp"

class OmView2dManager {
private:
	inline static OmView2dManagerImpl& impl(){
		return OmProject::Globals().View2dManagerImpl();
	}

public:
	template <typename T>
	inline static void AddTaskBack(const T& task){
		impl().AddTaskBack(task);
	}
};

#endif
