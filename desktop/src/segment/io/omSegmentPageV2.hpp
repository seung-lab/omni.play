#pragma once
#include "precomp.h"

#include "datalayer/fs/omFile.hpp"
#include "datalayer/fs/omFileNames.hpp"
#include "segment/omSegmentTypes.h"
#include "utility/malloc.hpp"
#include "volume/omSegmentationFolder.h"

class OmSegmentPageV2 {
 private:
  OmSegmentation* const vol_;
  const om::common::PageNum pageNum_;
  const uint32_t pageSize_;

 public:
  OmSegmentPageV2(OmSegmentation* vol, const om::common::PageNum pageNum,
                  const uint32_t pageSize)
      : vol_(vol), pageNum_(pageNum), pageSize_(pageSize) {}

  std::shared_ptr<OmSegmentDataV3> Read() {
    log_infos << "rewriting segment page " << pageNum_ << " from ver2 to ver3";

    QFile file(memMapPathQStrV2());
    om::file::old::openFileRO(file);
    OmSegmentDataV2* oldData = om::file::old::mapFile<OmSegmentDataV2>(&file);

    std::shared_ptr<OmSegmentDataV3> ret =
        om::mem::Malloc<OmSegmentDataV3>::NumElements(pageSize_,
                                                      om::mem::ZeroFill::ZERO);
    OmSegmentDataV3* newSegmentData = ret.get();

    for (uint32_t i = 0; i < pageSize_; ++i) {
      newSegmentData[i].value = oldData[i].value;
      newSegmentData[i].color = oldData[i].color;
      newSegmentData[i].size = oldData[i].size;
      newSegmentData[i].bounds = oldData[i].bounds;

      if (oldData[i].immutable) {
        newSegmentData[i].listType = om::common::SegListType::VALID;
      } else {
        newSegmentData[i].listType = om::common::SegListType::WORKING;
      }
    }

    return ret;
  }

 private:
  std::string memMapPathV2() { return memMapPathQStrV2().toStdString(); }

  QString memMapPathQStrV2() {
    const QString fname =
        QString("segment_page%2.%3").arg(pageNum_).arg("data");

    return QString::fromStdString(
        vol_->Folder()->GetVolSegmentsPathAbs(fname.toStdString()));
  }
};
