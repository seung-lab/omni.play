#pragma once
#include "precomp.h"

#include "datalayer/omDataWrapper.h"
#include "utility/omFileHelpers.h"
#include "volume/build/omMSTImportHdf5.hpp"
#include "volume/build/omMSTImportWatershed.hpp"
#include "volume/build/omVolumeBuilderBase.hpp"
#include "volume/build/omWatershedMetadata.hpp"
#include "volume/omSegmentation.h"
#include "volume/omVolumeTypes.hpp"

template <typename VOL>
class OmVolumeBuilderWatershed : public OmVolumeBuilderBase<VOL> {
 private:
  VOL& vol_;
  const QString fnp_;

  OmWatershedMetadata metadata_;

 public:
  OmVolumeBuilderWatershed(VOL& vol, const QFileInfo& file)
      : OmVolumeBuilderBase<VOL>(vol), vol_(vol), fnp_(file.filePath()) {
    metadata_.ReadMetata(fnp_);
  }

 protected:
  virtual void importSourceData() {
    setDataType();

    //      checkMipDims();

    moveFiles();
  }

  virtual Vector3i getMip0Dims() { return metadata_.GetMip0Dims(); }

  virtual bool loadDendrogram(OmSegmentation& vol) {
    OmMSTImportWatershed mstImport(&vol);

    return mstImport.Import(metadata_.MstFileName(), metadata_.MstBitsPerNode(),
                            metadata_.MstNumEdges());
  }

 private:
  void setDataType() {
    int colorDepth = metadata_.GetColorDepth();
    OmDataWrapperPtr nullData = makeNullDataWrapper(colorDepth);
    vol_.SetVolDataType(nullData->getVolDataType());
  }

  OmDataWrapperPtr makeNullDataWrapper(const int bpp) {
    switch (bpp) {
      case 32:
        return OmDataWrapper<uint32_t>::produceNull();
      default:
        throw om::IoException("unknown bpp");
    }
  }

  void moveFiles() {
    const std::vector<int> mipLevels = metadata_.GetMipLevels();

    FOR_EACH(iter, mipLevels) {
      const int mipLevel = *iter;

      const QString in_fnp = metadata_.GetMipLevelFileName(mipLevel);

      const QString out_fnp =
          vol_.VolPaths().Data(mipLevel, vol_.getVolDataType()).c_str();

      OmFileHelpers::MoveFile(in_fnp, out_fnp);
    }
  }
};
