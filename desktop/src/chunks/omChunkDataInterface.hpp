#pragma once
#include "precomp.h"

#include "datalayer/omDataWrapper.h"

namespace om {
namespace chunk {

class dataInterface {
 public:
  virtual std::shared_ptr<uint8_t> ExtractDataSlice8bit(
      const om::common::ViewType, const int) = 0;

  virtual ~dataInterface() {}

  virtual void CopyInTile(const int sliceOffset, uchar const* const bits) = 0;
  virtual void CopyInChunkData(OmDataWrapperPtr hdf5) = 0;
  virtual OmDataWrapperPtr CopyOutChunkData() = 0;

  virtual double GetMinValue() = 0;
  virtual double GetMaxValue() = 0;

  virtual bool Compare(dataInterface const* const other) = 0;
};

}  // namespace chunk
}  // namespace om
