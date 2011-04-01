#ifndef OM_SEGMENT_PAGE_V2_HPP
#define OM_SEGMENT_PAGE_V2_HPP

#include "datalayer/fs/omFileNames.hpp"
#include "segment/omSegmentTypes.h"
#include "utility/omSmartPtr.hpp"
#include "datalayer/fs/omFile.hpp"

class OmSegmentPageV2 {
private:
    OmSegmentation *const segmentation_;
    const PageNum pageNum_;
    const uint32_t pageSize_;

public:
    OmSegmentPageV2(OmSegmentation* segmentation, const PageNum pageNum,
                    const uint32_t pageSize)
        : segmentation_(segmentation)
        , pageNum_(pageNum)
        , pageSize_(pageSize)
    {}

    boost::shared_ptr<OmSegmentDataV3> Read()
    {
        printf("rewriting segment page %d from ver2 to ver3\n", pageNum_);

        QFile file(memMapPathQStrV2());
        om::file::openFileRO(file);
        OmSegmentDataV2* oldData = om::file::mapFile<OmSegmentDataV2>(&file);

        boost::shared_ptr<OmSegmentDataV3> ret =
            OmSmartPtr<OmSegmentDataV3>::MallocNumElements(pageSize_, om::ZERO_FILL);
        OmSegmentDataV3* newSegmentData = ret.get();

        for(uint32_t i = 0; i < pageSize_; ++i)
        {
            newSegmentData[i].value = oldData[i].value;
            newSegmentData[i].color = oldData[i].color;
            newSegmentData[i].size = oldData[i].size;
            newSegmentData[i].bounds = oldData[i].bounds;

            if(oldData[i].immutable){
                newSegmentData[i].listType = om::VALID;
            } else {
                newSegmentData[i].listType = om::WORKING;
            }
        }

        return ret;
    }

private:

   std::string memMapPathV2(){
        return memMapPathQStrV2().toStdString();
    }

    QString memMapPathQStrV2(){
        const QString volPath = OmFileNames::MakeVolSegmentsPath(segmentation_);
        const QString fullPath = QString("%1segment_page%2.%3")
            .arg(volPath)
            .arg(pageNum_)
            .arg("data");

        return fullPath;
    }
};

#endif
