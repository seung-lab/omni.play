#ifndef OM_COLORIZERS_HPP
#define OM_COLORIZERS_HPP

#include "zi/omMutex.h"

class OmViewGroupState;

class OmColorizers {
private:
    OmViewGroupState *const vgs_;

    zi::mutex lock_;
    std::vector<boost::shared_ptr<OmSegmentColorizer> > colorizers_;

public:
    OmColorizers(OmViewGroupState* vgs)
        : vgs_(vgs)
    {
        colorizers_.resize(SCC_NUMBER_OF_ENUMS);
    }

    inline boost::shared_ptr<OmColorRGBA> ColorTile(uint32_t const* imageData,
                                                    const Vector2i& dims,
                                                    const OmTileCoord& key)
    {
        const OmSegmentColorCacheType sccType =
            key.getSegmentColorCacheType();

        {
            zi::guard g(lock_);
            if(!colorizers_[sccType]){
                setupColorizer(dims, key, sccType);
            }
        }

        return colorizers_[ sccType ]->ColorTile(imageData);
    }

private:
    void setupColorizer(const Vector2i& dims,
                        const OmTileCoord& key,
                        const OmSegmentColorCacheType sccType)
    {
        if(SEGMENTATION != key.getVolume()->getVolumeType()){
            throw OmIoException("can only color segmentations");
        }

        SegmentationDataWrapper sdw(key.getVolume()->getID());

        colorizers_[ sccType ] =
            boost::make_shared<OmSegmentColorizer>(sdw.Segments(),
                                                   sccType,
                                                   dims,
                                                   vgs_);
    }
};

#endif
