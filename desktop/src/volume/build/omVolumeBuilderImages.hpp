#pragma once
#include "precomp.h"

#include "volume/build/omCompactVolValues.hpp"
#include "volume/build/omVolumeBuilderBase.hpp"
#include "volume/build/omDataCopierImages.hpp"

template <typename VOL>
class OmVolumeBuilderImages : public OmVolumeBuilderBase<VOL> {
 private:
  VOL* const vol_;
  const std::vector<QFileInfo> files_;

 public:
  OmVolumeBuilderImages(VOL* vol, const std::vector<QFileInfo> files)
      : OmVolumeBuilderBase<VOL>(vol), vol_(vol), files_(files) {}

 private:
  virtual void importSourceData() {
    OmDataCopierImages<VOL> importer(vol_, files_);
    importer.Import();
  }

  virtual Vector3i getMip0Dims() {
    QImage img(files_[0].absoluteFilePath());
    const Vector3i dims(img.width(), img.height(), files_.size());
    log_infos << "dims are " << dims;
    return dims;
  }

  virtual void rewriteMip0Volume(OmSegmentation* vol) {
    log_infos << "rewriting segment IDs...";

    OmCompactVolValues rewriter(vol);
    rewriter.Rewrite();

    log_infos << "done!";
  }
};
