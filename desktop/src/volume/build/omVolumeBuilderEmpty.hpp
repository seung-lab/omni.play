#pragma once
#include "precomp.h"

#include "volume/build/omVolumeBuilderBase.hpp"

template <typename VOL>
class OmVolumeBuilderEmpty : public OmVolumeBuilderBase<VOL> {
 private:
  VOL& vol_;

 public:
  OmVolumeBuilderEmpty(VOL& vol) : OmVolumeBuilderBase<VOL>(vol), vol_(vol) {}

 protected:
  virtual void importSourceData() {
    log_infos << "WARNING: assuming int8 typename";
    OmVolumeAllocater::AllocateData(&vol_, om::common::DataType::INT8);
  }

  virtual Vector3i getMip0Dims() {
    log_infos << "WARNING: assuming hard-coded dims of 4864, 20992, 12982";
    return Vector3i(4864, 20992, 12928);
  }
};
