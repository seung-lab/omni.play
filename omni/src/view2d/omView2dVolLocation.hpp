#ifndef OM_VIEW2D_VOL_LOCATION_HPP
#define OM_VIEW2D_VOL_LOCATION_HPP

#include "view2d/omView2dConverters.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupView2dState.hpp"
#include "volume/omMipVolume.h"

class OmView2dVolLocation {
private:
    OmMipVolume *const vol_;
    OmViewGroupView2dState *const v2dState_;
    const ViewType viewType_;

    const Vector3f resVec_;
    const float resolutionInThisPlane_;

public:
    OmView2dVolLocation(OmMipVolume* vol, OmViewGroupState* vgs,
                        const ViewType viewType)
        : vol_(vol)
        , v2dState_(vgs->View2dState())
        , viewType_(viewType)
        , resVec_(vol_->Coords().GetDataResolution())
        , resolutionInThisPlane_(getRes())
    {}

// location in actual data volume
    inline float DataDepth() const {
        return v2dState_->GetScaledSliceDepth(viewType_) / resolutionInThisPlane_;
    }

    inline void SetDataDepth(const float depth){
        v2dState_->SetScaledSliceDepth(viewType_, depth * resolutionInThisPlane_);
    }

    inline Vector3f DataLocation() const {
        return v2dState_->GetScaledSliceDepth() / resVec_;
    }

    inline void SetDataLocation(const Vector3f& vec){
        v2dState_->SetScaledSliceDepth(vec * resVec_);
    }

    void SetDataMinAndMax(const DataCoord& minData, const DataCoord& maxData)
    {
        const DataCoord min = minData * resVec_;
        const DataCoord max = maxData * resVec_;

        if(viewType_ == YZ_VIEW) {
            v2dState_->SetViewSliceMax(viewType_, max.y, max.x);
            v2dState_->SetViewSliceMin(viewType_, min.y, min.x);
        } else {
            v2dState_->SetViewSliceMax(viewType_, max.x, max.y);
            v2dState_->SetViewSliceMin(viewType_, min.x, min.y);
        }
    }

// location scaled by resolution factors
    inline float ScaledDepth() const {
        return v2dState_->GetScaledSliceDepth(viewType_);
    }

    inline Vector3f ScaledLocation() const {
        return v2dState_->GetScaledSliceDepth();
    }

   inline void SetScaledLocation(const Vector3f& vec) const {
       v2dState_->SetScaledSliceDepth(vec);
    }

private:
    inline float getRes() const {
        return OmView2dConverters::GetViewTypeDepth(resVec_, viewType_);
    }
};

#endif
