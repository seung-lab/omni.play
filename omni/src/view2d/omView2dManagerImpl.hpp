#ifndef OM_VIEW2D_MANAGER_IMPL_HPP
#define OM_VIEW2D_MANAGER_IMPL_HPP

#include "utility/omThreadPool.hpp"

class OmView2dManagerImpl {
private:
    OmThreadPool threadPool_;

public:
    OmView2dManagerImpl(){
        threadPool_.start();
    }

    ~OmView2dManagerImpl(){
        threadPool_.join();
    }

    template <typename T>
    inline void AddTaskBack(const T& task){
        threadPool_.addTaskBack(task);
    }
};

#endif
