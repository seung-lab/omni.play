#pragma once

#include "datalayer/fs/omFile.hpp"
#include "datalayer/fs/omFileNames.hpp"
#include "segment/omSegmentTypes.h"
#include "utility/omSmartPtr.hpp"
#include "volume/omSegmentationFolder.h"

class OmSegmentPageV2 {
private:
    OmSegmentation *const vol_;
    const om::common::PageNum pageNum_;
    const uint32_t pageSize_;

public:
    OmSegmentPageV2(OmSegmentation* vol, const om::common::PageNum pageNum, const uint32_t pageSize)
        : vol_(vol)
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
            OmSmartPtr<OmSegmentDataV3>::MallocNumElements(pageSize_, om::common::ZERO_FILL);
        OmSegmentDataV3* newSegmentData = ret.get();

        for(uint32_t i = 0; i < pageSize_; ++i)
        {
            newSegmentData[i].value = oldData[i].value;
            newSegmentData[i].color = oldData[i].color;
            newSegmentData[i].size = oldData[i].size;
            newSegmentData[i].bounds = oldData[i].bounds;

            if(oldData[i].immutable){
                newSegmentData[i].listType = om::common::VALID;
            } else {
                newSegmentData[i].listType = om::common::WORKING;
            }
        }

        return ret;
    }

private:

   std::string memMapPathV2(){
        return memMapPathQStrV2().toStdString();
    }

    QString memMapPathQStrV2()
    {
        const QString fname = QString("segment_page%2.%3")
            .arg(pageNum_)
            .arg("data");

        return QString::fromStdString(
            vol_->Folder()->GetVolSegmentsPathAbs(fname.toStdString())
            );
    }
};

