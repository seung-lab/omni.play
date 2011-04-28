#ifndef OM_COLORIZERS_HPP
#define OM_COLORIZERS_HPP

#include "zi/omMutex.h"

#include <boost/array.hpp>

class OmViewGroupState;

class OmColorizers {
private:
    OmViewGroupState *const vgs_;

    zi::spinlock lock_;
    boost::array<OmSegmentColorizer*, SCC_NUMBER_OF_ENUMS> colorizers_;

public:
    OmColorizers(OmViewGroupState* vgs)
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

    inline OmPooledTile<OmColorARGB>* ColorTile(uint32_t const*const imageData,
                                                const int tileDim,
                                                const OmTileCoord& key)
    {
        const OmSegmentColorCacheType sccType =
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
                        const OmSegmentColorCacheType sccType)
    {
        if(SEGMENTATION != key.getVolume()->getVolumeType()){
            throw OmIoException("can only color segmentations");
        }

        SegmentationDataWrapper sdw(key.getVolume()->getID());

        colorizers_[ sccType ] = new OmSegmentColorizer(sdw.Segments(),
                                                        sccType,
                                                        tileDim,
                                                        vgs_);
    }
};

#endif
