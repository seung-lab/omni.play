#include "project/omProject.h"
#include "segment/omSegments.h"
#include "utility/omFileHelpers.h"
#include "volume/omSegmentation.h"
#include "volume/omSegmentationFolder.h"
#include "volume/omSegmentationLoader.h"
#include "volume/io/omVolumeData.h"
#include "volume/build/omVolumeProcessor.h"

void om::segmentation::loader::LoadSegmentPages(
    OmPagingPtrStore& ps, QSet<om::common::PageNum>& validPageNumbers,
    uint32_t size) {
  if (OmProject::GetFileVersion() < 17) {
    vol_->Folder().MakeUserSegmentsFolder();

    ps.pageSize_ = size;

    FOR_EACH(iter, validPageNumbers) { ps.validPageNums_.insert(*iter); }

    ps.storeMetadata();  // segment data hasn't been loaded yet, so can't Flush
                         // pages
  }

  if (vol_->IsBuilt()) {
    const QString fullPath =
        QString::fromStdString(vol_->Folder().GetVolSegmentsPathAbs());

    if (OmFileHelpers::IsFolderEmpty(fullPath)) {
      rebuildSegments();

    } else {
      ps.loadAllSegmentPages();
    }
  }
}

void om::segmentation::loader::LoadSegmentPages(OmPagingPtrStore& ps) {
  if (vol_->IsBuilt()) {
    const QString fullPath =
        QString::fromStdString(vol_->Folder().GetVolSegmentsPathAbs());

    if (OmFileHelpers::IsFolderEmpty(fullPath)) {
      rebuildSegments();

    } else {
      ps.loadAllSegmentPages();
    }
  }
}

void om::segmentation::loader::rebuildSegments() {
  log_infos << "no segment folder; rebuild segment data?\n==> (y/N)  "
            << std::flush;

  std::string answer;
  std::getline(std::cin, answer);

  om::string::downcase(answer);

  if (om::string::startsWith(answer, "y")) {
    vol_->LoadVolData();

    vol_->VolData().downsample(vol_);

    vol_->Segments()->StartCaches();

    OmVolumeProcessor processor;
    processor.BuildThreadedVolume(vol_);

    vol_->Segments()->Flush();

    vol_->MSTUserEdges()->Save();
  }
}
