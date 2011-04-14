#include "chunks/omChunk.h"
#include "chunks/omChunkData.h"
#include "chunks/omRawChunk.hpp"
#include "chunks/omRawChunkSlicer.hpp"
#include "tiles/omTileFilters.hpp"
#include "utility/image/omImage.hpp"
#include "utility/omChunkVoxelWalker.hpp"
#include "volume/io/omVolumeData.h"

OmChunkData::OmChunkData(OmMipVolume* vol,
                         OmChunk* chunk)
    : vol_(vol)
    , chunk_(chunk)
{}

OmRawDataPtrs& OmChunkData::getRawData(){
    return chunk_->DataPtr()->GetRawData();
}

class ExtractDataSlice8bitVisitor
    : public boost::static_visitor<OmPooledTile<uint8_t>*>{
public:
    ExtractDataSlice8bitVisitor(const ViewType plane, const int depth)
        : plane_(plane)
        , depth_(depth)
    {}

    template <typename T>
    OmPooledTile<uint8_t>* operator()(T* d) const
    {
        boost::scoped_ptr<OmPooledTile<T> > rawTile(getRawSlice(d));
        OmTileFilters<T> filter(128);
        return filter.recastToUint8(rawTile.get());
    }

    OmPooledTile<uint8_t>* operator()(uint8_t* d) const {
        return getRawSlice(d);
    }

    OmPooledTile<uint8_t>* operator()(float* d) const
    {
        boost::scoped_ptr<OmPooledTile<float> > rawTile(getRawSlice(d));

        OmTileFilters<float> filter(128);

        float mn = 0.0;
        float mx = 1.0;

        // TODO: use actual range in channel data
        // mpMipVolume->GetBounds(mx, mn);

        return filter.rescaleAndCast<uint8_t>(rawTile.get(), mn, mx, 255.0);
    }

private:
    const ViewType plane_;
    const int depth_;

    template <typename T>
    inline OmPooledTile<T>* getRawSlice(T* d) const
    {
        OmRawChunkSlicer<T> slicer(128, d);

        OmProject::Globals().FileReadSemaphore().acquire(1);
        OmPooledTile<T>* tile = slicer.GetCopyAsPooledTile(plane_, depth_);
        OmProject::Globals().FileReadSemaphore().release(1);

        return tile;
    }

};
OmPooledTile<uint8_t>* OmChunkData::ExtractDataSlice8bit(const ViewType plane,
                                                      const int offset)
{
    return boost::apply_visitor(ExtractDataSlice8bitVisitor(plane, offset),
                                getRawData());
}


class CopyInDataVisitor : public boost::static_visitor<>{
public:
    CopyInDataVisitor(const int sliceOffset, uchar* bits)
        : sliceOffset_(sliceOffset)
        , bits_(bits) {}

    template <typename T>
    void operator()(T* d) const
    {
        const int advance = (128*128*(sliceOffset_%128))*sizeof(T);
        memcpy(d+advance,
               bits_,
               128*128*sizeof(T));
    }
private:
    const int sliceOffset_;
    uchar* bits_;
};
void OmChunkData::copyInTile(const int sliceOffset, uchar* bits)
{
    boost::apply_visitor(CopyInDataVisitor(sliceOffset, bits),
                         getRawData());
}


class CopyInChunkDataVisitor : public boost::static_visitor<>{
public:
    CopyInChunkDataVisitor(OmChunk* chunk,
                           OmDataWrapperPtr hdf5)
        : chunk_(chunk),
          hdf5_(hdf5)
    {}

    template <typename T>
    void operator()(T* d) const
    {
        T* dataHDF5 = hdf5_->getPtr<T>();

        memcpy(d,
               dataHDF5,
               128*128*128*sizeof(T));
    }

private:
    OmChunk* chunk_;
    OmDataWrapperPtr hdf5_;
};
void OmChunkData::CopyInChunkData(OmDataWrapperPtr hdf5)
{
    boost::apply_visitor(CopyInChunkDataVisitor(chunk_, hdf5),
                         getRawData());
}


class CompareVisitor : public boost::static_visitor<bool>{
public:
    CompareVisitor(OmRawDataPtrs& other)
        : other_(other) {}

    template <typename T>
    bool operator()(T* d) const
    {
        T* otherD = boost::get<T*>(other_);
        if(NULL == otherD){
            printf("\terror: chunk types differ...\n");
            return false;
        }
        for(int i =0; i < 128*128*128; ++i){
            if(otherD[i] != d[i]){
                return false;
            }
        }

        return true;
    }

private:
    OmRawDataPtrs& other_;
};
bool OmChunkData::Compare(OmChunkData* other)
{
    return boost::apply_visitor(CompareVisitor(other->getRawData()),
                                getRawData());
}

class GetMaxVisitor : public boost::static_visitor<double>{
public:
    template <typename T>
    double operator()(T* d) const
    {
        OmImage<T,3> data(OmExtents[128][128][128],
                          d);
        return static_cast<double>(data.getMax());
    }
};
double OmChunkData::GetMaxValue()
{
    return boost::apply_visitor(GetMaxVisitor(),
                                getRawData());
}

class GetMinVisitor : public boost::static_visitor<double>{
public:
    template <typename T>
    double operator()(T* d) const
    {
        OmImage<T,3> data(OmExtents[128][128][128],
                          d);
        return data.getMin();
    }
};
double OmChunkData::GetMinValue()
{
    return boost::apply_visitor(GetMinVisitor(),
                                getRawData());
}


class CopyOutChunkDataVisitor
    : public boost::static_visitor<OmDataWrapperPtr>{
public:
    CopyOutChunkDataVisitor(OmMipVolume* vol,
                            OmChunk* chunk)
        : vol_(vol)
        , chunk_(chunk)
    {}

    template <typename T>
    OmDataWrapperPtr operator()(T*) const
    {
        OmRawChunk<T> rawChunk(vol_, chunk_->GetCoordinate());

        om::shared_ptr<T> data = rawChunk.SharedPtr();
        return om::ptrs::Wrap(data);
    }

private:
    OmMipVolume *const vol_;
    OmChunk *const chunk_;
};
OmDataWrapperPtr OmChunkData::CopyOutChunkData()
{
    return boost::apply_visitor(CopyOutChunkDataVisitor(vol_, chunk_),
                                getRawData());
}
