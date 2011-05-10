#pragma once

#include "chunks/details/omPtrToChunkDataMemMapVol.h"
#include "chunks/omChunkDataInterface.hpp"
#include "chunks/omRawChunk.hpp"
#include "chunks/omRawChunkSlicer.hpp"
#include "datalayer/omDataWrapper.h"
#include "tiles/omTileFilters.hpp"
#include "utility/image/omImage.hpp"
#include "utility/omChunkVoxelWalker.hpp"
#include "volume/io/omVolumeData.h"

namespace om {
namespace chunk {

template <typename DATA>
class dataImpl : public dataInterface {
private:
    OmMipVolume *const vol_;
    const OmChunkCoord coord_;
    const int numElementsPerSlice_;
    const int numElementsPerChunk_;
    const boost::scoped_ptr<OmPtrToChunkDataBase> ptrToChunkData_;

    DATA* rawData(){
        return ptrToChunkData_->GetRawData(static_cast<DATA*>(NULL));
    }

public:
    dataImpl(OmMipVolume* vol, const OmChunkCoord& coord)
        : vol_(vol)
        , coord_(coord)
        , numElementsPerSlice_(128*128)
        , numElementsPerChunk_(numElementsPerSlice_*128)
        , ptrToChunkData_(new OmPtrToChunkDataMemMapVol(vol, coord))
    {}

public:
    void CopyInTile(const int sliceOffset, uchar* bits)
    {
        const int advance = (numElementsPerSlice_ * (sliceOffset % 128)) * sizeof(DATA);

        memcpy(rawData() + advance,
               bits,
               numElementsPerSlice_ * sizeof(DATA));
    }

    void CopyInChunkData(OmDataWrapperPtr hdf5)
    {
        DATA* dataHDF5 = hdf5->getPtr<DATA>();

        memcpy(rawData(),
               dataHDF5,
               numElementsPerChunk_ * sizeof(DATA));
    }

    bool Compare(dataInterface* chunkInter)
    {
        dataImpl* other = reinterpret_cast<dataImpl*>(chunkInter);

        DATA* otherD = other->rawData();
        DATA* d = rawData();

        for(int i = 0; i < numElementsPerChunk_; ++i)
        {
            if(otherD[i] != d[i]){
                return false;
            }
        }

        return true;
    }

    double GetMaxValue() {
        return *std::max_element(rawData(), rawData() + numElementsPerChunk_);
    }

    double GetMinValue() {
        return *std::min_element(rawData(), rawData() + numElementsPerChunk_);
    }

    OmDataWrapperPtr CopyOutChunkData()
    {
        OmRawChunk<DATA> rawChunk(vol_, coord_);

        om::shared_ptr<DATA> data = rawChunk.SharedPtr();

        return om::ptrs::Wrap(data);
    }

    OmPooledTile<uint8_t>* ExtractDataSlice8bit(const ViewType plane, const int offset){
        return extractDataSlice8bit(rawData(), plane, offset);
    }

private:
    template <typename T>
    OmPooledTile<uint8_t>* extractDataSlice8bit(T* d, const ViewType plane, const int offset)
    {
        boost::scoped_ptr<OmPooledTile<T> > rawTile(getRawSlice(d, plane, offset));
        OmTileFilters<T> filter(128);
        return filter.recastToUint8(rawTile.get());
    }

    OmPooledTile<uint8_t>* extractDataSlice8bit(uint8_t* d, const ViewType plane, const int offset){
        return getRawSlice(d, plane, offset);
    }

    OmPooledTile<uint8_t>* extractDataSlice8bit(float* d, const ViewType plane, const int offset)
    {
        boost::scoped_ptr<OmPooledTile<float> > rawTile(getRawSlice(d, plane, offset));

        OmTileFilters<float> filter(128);

        float mn = 0.0;
        float mx = 1.0;

        // TODO: use actual range in channel data
        // mpMipVolume->GetBounds(mx, mn);

        return filter.rescaleAndCast<uint8_t>(rawTile.get(), mn, mx, 255.0);
    }

    template <typename T>
    inline OmPooledTile<T>* getRawSlice(T* d, const ViewType plane, const int depth) const
    {
        OmRawChunkSlicer<T> slicer(128, d);

        OmProject::Globals().FileReadSemaphore().acquire(1);
        OmPooledTile<T>* tile = slicer.GetCopyAsPooledTile(plane, depth);
        OmProject::Globals().FileReadSemaphore().release(1);

        return tile;
    }
};

} // namespace chunk
} // namespace om
