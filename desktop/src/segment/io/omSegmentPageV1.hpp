#pragma once

#include "common/common.h"
#include "datalayer/archive/old/omDataArchiveSegment.h"
#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "segment/omSegmentTypes.h"

#include "utility/malloc.hpp"

class OmSegmentPageV1 {
private:
    OmSegmentation *const segmentation_;
    const PageNum pageNum_;
    const uint32_t pageSize_;

public:
    OmSegmentPageV1(OmSegmentation* segmentation, const PageNum pageNum,
                    const uint32_t pageSize)
        : segmentation_(segmentation)
        , pageNum_(pageNum)
        , pageSize_(pageSize)
    {}

    std::shared_ptr<OmSegmentDataV3> Read()
    {
        printf("rewriting segment page %d from HDF5\n", pageNum_);

        std::shared_ptr<OmSegmentDataV2> oldSegmentDataPtr =
            om::mem::Malloc<OmSegmentDataV2>::MallocNumElements(pageSize_,
                                                           om::common::ZeroMem::ZERO_FILL);

        OmSegmentDataV2* oldSegmentData = oldSegmentDataPtr.get();

        OmDataArchiveSegment::ArchiveRead(getOldHDF5path(),
                                          oldSegmentData,
                                          pageSize_);

        std::shared_ptr<OmSegmentDataV3> ret =
            om::mem::Malloc<OmSegmentDataV3>::MallocNumElements(pageSize_, om::common::ZeroMem::ZERO_FILL);
        OmSegmentDataV3* newSegmentData = ret.get();

        for(uint32_t i = 0; i < pageSize_; ++i)
        {
            newSegmentData[i].value = oldSegmentData[i].value;
            newSegmentData[i].color = oldSegmentData[i].color;
            newSegmentData[i].size = oldSegmentData[i].size;
            newSegmentData[i].bounds = oldSegmentData[i].bounds;

            if(oldSegmentData[i].immutable){
                newSegmentData[i].listType = om::common::SegListType::VALID;
            } else {
                newSegmentData[i].listType = om::common::SegListType::WORKING;
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

