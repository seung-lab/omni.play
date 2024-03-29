#pragma once
#include "precomp.h"

#include "chunks/omChunkCache.hpp"
#include "common/logging.h"
#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "utility/omTimer.hpp"
#include "utility/sortHelpers.h"
#include "volume/build/omVolumeAllocater.hpp"
#include "volume/build/omVolumeProcessor.h"
#include "volume/io/omVolumeData.h"
#include "volume/omMipVolume.h"
#include "volume/omSegmentation.h"
#include "headless/headlessImpl.hpp"

namespace om {
namespace rebuilder {

class segmentation {

 private:
  OmSegmentation* vol_;

 public:
  segmentation(OmSegmentation* vol) : vol_(vol) {}

  segmentation(const SegmentationDataWrapper& sdw) {
    vol_ = sdw.GetSegmentation();
  }

  void Rebuild() {
    setVolAsBuilding();

    OmVolumeAllocater::ReAllocateDownsampledVolumes(vol_);

    downsample();
    processVol();

    setVolAsBuilt();

    vol_->LoadVolData();

    HeadlessImpl::ClearMST(vol_->GetID());
  }

 private:
  virtual void downsample() { vol_->VolData().downsample(vol_); }

  virtual void processVol() {
    OmVolumeProcessor processor;
    processor.BuildThreadedVolume(*vol_);
  }

  void setVolAsBuilding() { vol_->SetBuildState(MIPVOL_BUILDING); }

  void setVolAsBuilt() { vol_->SetBuildState(MIPVOL_BUILT); }
};
}
}
