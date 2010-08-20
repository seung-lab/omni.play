#include "utility/image/omImage.hpp"
#include "volume/omChunkData.hpp"
#include "volume/omVolumeData.hpp"

OmChunkData::OmChunkData(OmMipVolume* vol,
			 OmMipChunk* chunk,
			 const OmMipChunkCoord &coord)
	: rawData(vol->volData->getChunkPtrRaw(coord))
	, vol_(vol)
	, chunk_(chunk)
	, coord_(coord)
{}


class ExtractDataSliceVisitor : public boost::static_visitor<void*>{
public:
	ExtractDataSliceVisitor(const ViewType plane, int offset)
		: plane(plane), offset(offset) {}

	template <typename T>
	void* operator()(T* d ) const {
		OmImage<T, 3, OmImageRefData> chunk(OmExtents[128][128][128], d);
		OmImage<T, 2> slice = chunk.getSlice(plane, offset);
		return slice.getMallocCopyOfData();
	}

	void* operator()(float* d ) const {
		OmImage<float, 3, OmImageRefData> chunk(OmExtents[128][128][128], d);
		OmImage<float, 2> sliceFloat = chunk.getSlice(plane, offset);
		float mn = 0.0;
		float mx = 1.0;
		//	  mpMipVolume->GetBounds(mx, mn);
		OmImage<unsigned char, 2> slice =
			sliceFloat.rescaleAndCast<unsigned char>(mn, mx, 255.0);
		return slice.getMallocCopyOfData();
	}
private:
	const ViewType plane;
	const int offset;
};
void* OmChunkData::ExtractDataSlice(const ViewType plane, const int offset)
{
	return boost::apply_visitor(ExtractDataSliceVisitor(plane, offset),
				    rawData);
}


class RefreshDirectDataValuesVisitor :
	public boost::static_visitor<OmSegSizeMapPtr>{
public:
	RefreshDirectDataValuesVisitor(OmMipChunk* chunk,
				       const bool computeSizes)
		: chunk_(chunk)
		, computeSizes_(computeSizes) {}

	template <typename T>
	OmSegSizeMapPtr operator()(T* d ) const{
		return doRefreshDirectDataValues(d);
	}
private:
	OmMipChunk *const chunk_;
	const bool computeSizes_;

	template <typename C>
	OmSegSizeMapPtr doRefreshDirectDataValues(C* data) const
	{
		OmSegSizeMapPtr sizes(new OmSegSizeMap());

		//for all voxels in the chunk
		for(int z = 0; z < 128; z++) {
			for(int y = 0; y < 128; y++) {
				for(int x = 0; x < 128; x++) {

					const OmSegID val = static_cast<OmSegID>(*data++);

					if(0 == val){ continue; }

					chunk_->mDirectlyContainedValues.insert(val);

					if(!computeSizes_){ continue; }

					++((*sizes)[val]);

					chunk_->addVoxelToBounds(val,
								 Vector3i(x,y,z));
				}
			}
		}
		return sizes;
	}
};
OmSegSizeMapPtr OmChunkData::RefreshDirectDataValues(const bool computeSizes)
{
	return boost::apply_visitor(RefreshDirectDataValuesVisitor(chunk_, computeSizes),
				    rawData);
}


class CopyInDataVisitor : public boost::static_visitor<>{
public:
	CopyInDataVisitor(const int sliceOffset, uchar* bits)
		: sliceOffset_(sliceOffset)
		, bits_(bits) {}

	template <typename T>
	void operator()(T* d ) const{
		const int advance = (128*128*(sliceOffset_%128))*sizeof(T);
		memcpy(d+advance,
		       (T*)bits_,
		       128*128*sizeof(T));
	}
private:
	const int sliceOffset_;
	uchar* bits_;
};
void OmChunkData::copyInTile(const int sliceOffset, uchar* bits)
{
	boost::apply_visitor(CopyInDataVisitor(sliceOffset, bits),
			     rawData);
}


class CopyDataFromMemMapToHDF5Visitor : public boost::static_visitor<>{
public:
	CopyDataFromMemMapToHDF5Visitor(OmMipChunk* chunk)
		: chunk_(chunk) {}

	template <typename T>
	void operator()(T* d) const {
		chunk_->RawWriteChunkData(d);
	}
private:
	OmMipChunk* chunk_;
};
void OmChunkData::copyChunkFromMemMapToHDF5()
{
	boost::apply_visitor(CopyDataFromMemMapToHDF5Visitor(chunk_),
			     rawData);
}


class CopyDataFromHDF5toMemMapVisitor : public boost::static_visitor<>{
public:
	CopyDataFromHDF5toMemMapVisitor(OmMipChunk* chunk,
					OmDataWrapperPtr hdf5)
		: chunk_(chunk), hdf5_(hdf5) {}

	template <typename T>
	void operator()(T* d) const {
		T* dataHDF5 = hdf5_->getPtr<T>();

		memcpy(d,
		       dataHDF5,
		       128*128*128*sizeof(T));
	}

private:
	OmMipChunk* chunk_;
	OmDataWrapperPtr hdf5_;
};
void OmChunkData::copyDataFromHDF5toMemMap()
{
	OmDataWrapperPtr hdf5 = chunk_->RawReadChunkDataHDF5();
	boost::apply_visitor(CopyDataFromHDF5toMemMapVisitor(chunk_, hdf5),
			     rawData);
}
void OmChunkData::copyDataFromHDF5toMemMap(OmDataWrapperPtr hdf5)
{
	boost::apply_visitor(CopyDataFromHDF5toMemMapVisitor(chunk_, hdf5),
			     rawData);
}


class GetOmImage32ChunkVisitor
	: public boost::static_visitor<OmImage<uint32_t, 3> >{
public:
	GetOmImage32ChunkVisitor(OmMipChunk* chunk)
		: chunk_(chunk) {}


	template <typename T>
	OmImage<uint32_t, 3> operator()(T* d) const {
		OmImage<T,3> data(OmExtents[128][128][128],
				  d);
		return data.recastToUint32();
	}

	OmImage<uint32_t, 3> operator()(uint32_t* d) const {
		return OmImage<uint32_t, 3>(OmExtents[128][128][128],
					    d);
	}

	OmImage<uint32_t, 3> operator()(float*) const {
		throw OmIoException("can't mesh float data!");
	}
private:
	OmMipChunk* chunk_;
};
OmImage<uint32_t, 3> OmChunkData::getOmImage32Chunk()
{
	return boost::apply_visitor(GetOmImage32ChunkVisitor(chunk_),
				    rawData);
}
