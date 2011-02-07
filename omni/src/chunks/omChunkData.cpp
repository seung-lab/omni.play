#include "chunks/omChunk.h"
#include "chunks/omChunkData.h"
#include "chunks/omRawChunk.hpp"
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
    : public boost::static_visitor<OmImage<uint8_t, 2> >{
public:
    ExtractDataSlice8bitVisitor(const ViewType plane, int offset)
        : plane(plane)
        , offset(offset)
    {}

    template <typename T>
    OmImage<uint8_t, 2> operator()(T* d) const
    {
        OmImage<T, 3, OmImageRefData> chunk(OmExtents[128][128][128], d);
        OmImage<T, 2> sliceT = chunk.getSlice(plane, offset);
        OmImage<uint8_t, 2> slice = sliceT.recastToUint8();
        return slice;
    }

    OmImage<uint8_t, 2> operator()(uint8_t* d) const
    {
        OmImage<uint8_t, 3, OmImageRefData> chunk(OmExtents[128][128][128], d);
        OmImage<uint8_t, 2> slice = chunk.getSlice(plane, offset);
        return slice;
    }

    OmImage<uint8_t, 2> operator()(float* d) const
    {
        OmImage<float, 3, OmImageRefData> chunk(OmExtents[128][128][128], d);
        OmImage<float, 2> sliceFloat = chunk.getSlice(plane, offset);
        float mn = 0.0;
        float mx = 1.0;
        //	  mpMipVolume->GetBounds(mx, mn);
        OmImage<uint8_t, 2> slice =
            sliceFloat.rescaleAndCast<uint8_t>(mn, mx, 255.0);
        return slice;
    }
private:
    const ViewType plane;
    const int offset;
};
OmImage<uint8_t, 2> OmChunkData::ExtractDataSlice8bit(const ViewType plane,
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

        boost::shared_ptr<T> data = rawChunk.SharedPtr();
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
