#ifndef OM_CHUNK_DATA_HPP
#define OM_CHUNK_DATA_HPP

#include "chunks/omChunkCoord.h"
#include "datalayer/omDataWrapper.h"
#include "volume/omVolumeTypes.hpp"

class OmChunk;
class OmMipVolume;
template <class> class OmPooledTile;

class OmChunkData {
public:
    OmChunkData(OmMipVolume*, OmChunk*);

    OmPooledTile<uint8_t>* ExtractDataSlice8bit(const ViewType, const int);

    void copyInTile(const int sliceOffset, uchar* bits);
    void CopyInChunkData(OmDataWrapperPtr hdf5);
    OmDataWrapperPtr CopyOutChunkData();

    double GetMinValue();
    double GetMaxValue();

    bool Compare(OmChunkData* other);

private:
    OmMipVolume *const vol_;
    OmChunk *const chunk_;

    OmRawDataPtrs& getRawData();
};

#endif
