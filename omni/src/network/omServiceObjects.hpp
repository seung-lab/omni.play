#ifndef OM_SERVICE_OBJECTS_HPP
#define OM_SERVICE_OBJECTS_HPP

#include "network/omAssembleTilesIntoSlice.hpp"
#include "segment/omSegmentSelected.hpp"

class OmServiceObjects {
private:
    boost::scoped_ptr<OmAssembleTilesIntoSlice> tileAssembler_;

public:
    OmServiceObjects()
        : tileAssembler_(new OmAssembleTilesIntoSlice())
    {}

    /**
     * returns UUID
     * creates
     *   /var/www/temp_omni_imgs/channel-1/UUID.png
     **/
    std::string MakeImgFileChannel(const int sliceNum){
        return tileAssembler_->MakeImgFileChannel(sliceNum);
    }

    /**
     * returns UUID
     * creates
     *   /var/www/temp_omni_imgs/segmenation-1/UUID.png
     **/
    std::string MakeImgFileSegmentation(const int sliceNum){
        return tileAssembler_->MakeImgFileSegmentation(sliceNum);
    }

    /**
     * returns UUID
     * creates
     *   /var/www/temp_omni_imgs/segmenation-1/UUID.png
     **/
    std::string SelectSegment(const OmID segmentationID, const int sliceNum,
                              const float x, float y,
                              const float w, const float h)
    {
        SegmentationDataWrapper sdw(segmentationID);
        OmSegmentation& vol = sdw.GetSegmentation();

        const AxisAlignedBoundingBox<int> volExtent = vol.Coords().GetDataExtent();

        const DataCoord coord(x * volExtent.getMax().x / w,
                              y * volExtent.getMax().y / h,
                              sliceNum);

        std::cout << "coord: " << coord << "\n";

        if(volExtent.contains(coord))
        {
            OmSegment* seg = vol.Segments()->findRoot(vol.GetVoxelValue(coord));

            if(seg)
            {
                seg->SetSelected(!seg->IsSelected(), true);
            }
        }

        return tileAssembler_->MakeImgFileSegmentation(sliceNum);
    }

    std::string GetMeshData(const OmID segmentationID, const OmSegID segID,
                            const int mipLevel, const int x, const int y, const int z)

    {
        std::cout << segmentationID << segID << mipLevel << x << y << z << "\n";
        return "fixme";
    }
};

#endif
