#pragma once

#include "zi/omMutex.h"

#include <boost/array.hpp>

class OmViewGroupState;

class om::common::Colorizers {
private:
    OmViewGroupState *const vgs_;

    zi::spinlock lock_;
    boost::array<OmSegmentColorizer*, om::common::SCC_NUMBER_OF_ENUMS> colorizers_;

public:
    om::common::Colorizers(OmViewGroupState* vgs)
        : vgs_(vgs)
    {
        std::fill(colorizers_.begin(), colorizers_.end(),
                  static_cast<OmSegmentColorizer*>(NULL));
    }

    ~OmColorizers()
    {
        FOR_EACH(iter, colorizers_){
            delete *iter;
        }
    }

    inline OmPooledTile<om::common::ColorARGB>* ColorTile(uint32_t const*const imageData,
                                                const int tileDim,
                                                const OmTileCoord& key)
    {
        const om::common::SegmentColorCacheType sccType =
            key.getSegmentColorCacheType();

        {
            zi::guard g(lock_);
            if(!colorizers_[sccType]){
                setupColorizer(tileDim, key, sccType);
            }
        }

        return colorizers_[ sccType ]->ColorTile(imageData);
    }

private:
    void setupColorizer(const int tileDim,
                        const OmTileCoord& key,
                        const om::common::SegmentColorCacheType sccType)
    {
        if(SEGMENTATION != key.getVolume()->getVolumeType()){
            throw om::IoException("can only color segmentations");
        }

        SegmentationDataWrapper sdw(key.getVolume()->getID());

        colorizers_[ sccType ] = new OmSegmentColorizer(sdw.Segments(),
                                                        sccType,
                                                        tileDim,
                                                        vgs_);
    }
};

