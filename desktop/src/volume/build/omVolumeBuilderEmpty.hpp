#pragma once

#include "volume/build/omVolumeBuilderBase.hpp"

template <typename VOL>
class OmVolumeBuilderEmpty : public OmVolumeBuilderBase<VOL> {
 private:
  VOL* const vol_;

 public:
  OmVolumeBuilderEmpty(VOL* vol) : OmVolumeBuilderBase<VOL>(vol), vol_(vol) {}

 protected:
  virtual void importSourceData() {
    std::cout << "WARNING: assuming int8 typename\n";
    OmVolumeAllocater::AllocateData(vol_, OmVolDataType::INT8);
  }

  virtual Vector3i getMip0Dims() {
    std::cout << "WARNING: assuming hard-coded dims of 4864, 20992, 12982\n";
    return Vector3i(4864, 20992, 12928);
  }
};
