#pragma once
#include "precomp.h"

#include "volume/io/omVolumeData.h"
#include "volume/omMipVolume.h"
#include "chunks/details/omPtrToChunkDataBase.hpp"

namespace om {
namespace chunk {

template <typename DATA>
class ptrToChunkDataMemMapVol : public ptrToChunkDataBase {
 private:
  DATA* const rawData_;

 public:
  ptrToChunkDataMemMapVol(OmMipVolume* vol, const om::coords::Chunk& coord)
      : rawData_(boost::get<DATA*>(vol->VolData().getChunkPtrRaw(coord))) {}

  using ptrToChunkDataBase::GetRawData;

  virtual DATA* GetRawData(DATA*) { return rawData_; }
};

}  // namespace chunk
}  // namespace om
