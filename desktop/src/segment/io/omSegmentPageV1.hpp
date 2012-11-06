#pragma once

#include "common/common.h"
#include "datalayer/archive/old/omDataArchiveSegment.h"
#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "segment/omSegmentTypes.h"

#include "utility/omSmartPtr.hpp"

class OmSegmentPageV1 {
private:
    OmSegmentation *const segmentation_;
    const om::common::PageNum pageNum_;
    const uint32_t pageSize_;

public:
    OmSegmentPageV1(OmSegmentation* segmentation, const om::common::PageNum pageNum,
                    const uint32_t pageSize)
        : segmentation_(segmentation)
        , pageNum_(pageNum)
        , pageSize_(pageSize)
    {}

    boost::shared_ptr<OmSegmentDataV3> Read()
    {
        printf("rewriting segment page %d from HDF5\n", pageNum_);

        boost::shared_ptr<OmSegmentDataV2> oldSegmentDataPtr =
            OmSmartPtr<OmSegmentDataV2>::MallocNumElements(pageSize_,
                                                           om::common::ZERO_FILL);

        OmSegmentDataV2* oldSegmentData = oldSegmentDataPtr.get();

        OmDataArchiveSegment::ArchiveRead(getOldHDF5path(),
                                          oldSegmentData,
                                          pageSize_);

        boost::shared_ptr<OmSegmentDataV3> ret =
            OmSmartPtr<OmSegmentDataV3>::MallocNumElements(pageSize_, om::common::ZERO_FILL);
        OmSegmentDataV3* newSegmentData = ret.get();

        for(uint32_t i = 0; i < pageSize_; ++i)
        {
            newSegmentData[i].value = oldSegmentData[i].value;
            newSegmentData[i].color = oldSegmentData[i].color;
            newSegmentData[i].size = oldSegmentData[i].size;
            newSegmentData[i].bounds = oldSegmentData[i].bounds;

            if(oldSegmentData[i].immutable){
                newSegmentData[i].listType = om::common::VALID;
            } else {
                newSegmentData[i].listType = om::common::WORKING;
            }
        }

        return ret;
    }

private:
    OmDataPath getOldHDF5path(){
        return OmDataPaths::getSegmentPagePath(segmentation_->GetID(),
                                               pageNum_);
    }
};

