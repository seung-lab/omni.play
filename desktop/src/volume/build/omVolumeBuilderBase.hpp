#pragma once
#include "precomp.h"

#include "common/logging.h"
#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "utility/omTimer.hpp"
#include "utility/sortHelpers.h"
#include "volume/build/omVolumeProcessor.h"
#include "volume/io/omVolumeData.h"
#include "volume/omMipVolume.h"
#include "utility/dataWrappers.h"

template <typename VOL>
class OmVolumeBuilderBase {
 private:
  VOL& vol_;

 public:
  OmVolumeBuilderBase(VOL& vol) : vol_(vol) {}

  virtual ~OmVolumeBuilderBase() {}

  void Build() { build(vol_); }

  void BuildEmpty() { buildEmpty(vol_); }

 private:
  void build(OmChannel&) {
    setVolAsBuilding();

    checkChunkDims();
    updateMipProperties();
    importSourceData();

    setVolAsBuilt();

    downsample();
  }

  void build(OmSegmentation&) {
    setVolAsBuilding();

    checkChunkDims();
    updateMipProperties();
    importSourceData();

    rewriteMip0Volume(vol_);
    downsample();
    processVol();
    loadDendrogramWrapper(vol_);

    setVolAsBuilt();

    vol_.LoadVolData();
  }

  void build(OmAffinityChannel&) {
    // TODO: Check this process for completeness
    setVolAsBuilding();

    checkChunkDims();
    updateMipProperties();
    importSourceData();

    setVolAsBuilt();

    downsample();
  }

  void buildEmpty(OmChannel&) {
    setVolAsBuilding();

    checkChunkDims();
    updateMipProperties();
    importSourceData();

    setVolAsBuilt();
  }

 protected:
  virtual void importSourceData() = 0;

  virtual Vector3i getMip0Dims() = 0;

  virtual bool loadDendrogram(OmSegmentation&) { return false; }

  virtual void rewriteMip0Volume(OmSegmentation&) {}

  virtual void downsample() { vol_.VolData().downsample(&vol_); }

  virtual void processVol() {
    OmVolumeProcessor processor;
    processor.BuildThreadedVolume(vol_);
  }

 private:
  void loadDendrogramWrapper(OmSegmentation& vol) {
    log_infos << "************************";
    log_infos << "loading MST...";

    if (!loadDendrogram(vol)) {
      log_infos << "no MST found";
      log_infos << "************************";
      return;
    }

    log_infos << "Segmentation MST load done";
    log_infos << "************************";

    OmActions::ChangeMSTthreshold(vol.GetSDW(), 0.999);
    om::event::SegmentModified();
  }

  void setVolAsBuilding() { vol_.SetBuildState(MIPVOL_BUILDING); }

  void setVolAsBuilt() { vol_.SetBuildState(MIPVOL_BUILT); }

  void checkChunkDims() {
    if (vol_.Coords().ChunkDimensions().x % 2) {
      throw om::FormatException("chunk dimensions must be even");
    }
  }

  void updateMipProperties() {
    const Vector3i source_dims = getMip0Dims();

    if (vol_.Coords().DataDimensions() != source_dims) {
      vol_.Coords().SetDataDimensions(source_dims);
    }

    vol_.Coords().UpdateRootLevel();

    vol_.UpdateFromVolResize();
  }
};
