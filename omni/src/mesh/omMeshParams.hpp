#ifndef OM_MESH_PARAMS_HPP
#define OM_MESH_PARAMS_HPP

#include "zi/omUtility.h"

class OmMeshParams : private om::singletonBase<OmMeshParams> {
public:
    static void SetDownScallingFactor(const double factor){
        instance().factor_ = factor;
    }
    static double GetDownScallingFactor(){
        return instance().factor_;
    }

private:
    OmMeshParams()
        : factor_(2.5)
    {}

    double factor_;

    friend class zi::singleton<OmMeshParams>;
};

#endif
