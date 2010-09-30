#include "volume/omMipChunk.h"
#include "utility/image/omImage.hpp"
#include "volume/omChunkData.hpp"
#include "volume/omVolumeData.hpp"
#include "segment/omSegmentCache.h"
#include "utility/omChunkVoxelWalker.hpp"

OmChunkData::OmChunkData(OmMipVolume* vol,
						 OmMipChunk* chunk,
						 const OmMipChunkCoord &coord)
	: vol_(vol)
	, chunk_(chunk)
	, coord_(coord)
	, loadedData_(false)
{}

OmRawDataPtrs& OmChunkData::getRawData()
{
	if(!loadedData_){
		rawData_ = vol_->getVolData()->getChunkPtrRaw(coord_);
		loadedData_ = true;
	}
	return rawData_;
}


class ExtractDataSlice8bitVisitor
	: public boost::static_visitor<boost::shared_ptr<uint8_t> >{
public:
	ExtractDataSlice8bitVisitor(const ViewType plane, int offset)
		: plane(plane), offset(offset) {}

	template <typename T>
	boost::shared_ptr<uint8_t> operator()(T* d ) const {
		OmImage<T, 3, OmImageRefData> chunk(OmExtents[128][128][128], d);
		OmImage<T, 2> slice = chunk.getSlice(plane, offset);
		return slice.recastToUint8().getMallocCopyOfData();
	}

	boost::shared_ptr<uint8_t> operator()(uint8_t* d ) const {
		OmImage<uint8_t, 3, OmImageRefData> chunk(OmExtents[128][128][128], d);
		OmImage<uint8_t, 2> slice = chunk.getSlice(plane, offset);
		return slice.getMallocCopyOfData();
	}

	boost::shared_ptr<uint8_t> operator()(float* d ) const {
		OmImage<float, 3, OmImageRefData> chunk(OmExtents[128][128][128], d);
		OmImage<float, 2> sliceFloat = chunk.getSlice(plane, offset);
		float mn = 0.0;
		float mx = 1.0;
		//	  mpMipVolume->GetBounds(mx, mn);
		OmImage<uint8_t, 2> slice =
			sliceFloat.rescaleAndCast<uint8_t>(mn, mx, 255.0);
		return slice.getMallocCopyOfData();
	}
private:
	const ViewType plane;
	const int offset;
};
boost::shared_ptr<uint8_t> OmChunkData::ExtractDataSlice8bit(const ViewType plane,
															 const int offset)
{
	return boost::apply_visitor(ExtractDataSlice8bitVisitor(plane, offset),
								getRawData());
}


class ExtractDataSlice32bitVisitor
	: public boost::static_visitor<boost::shared_ptr<uint32_t> >{
public:
	ExtractDataSlice32bitVisitor(const ViewType plane, int offset)
		: plane(plane), offset(offset) {}

	template <typename T>
	boost::shared_ptr<uint32_t> operator()(T* d ) const {
		OmImage<T, 3, OmImageRefData> chunk(OmExtents[128][128][128], d);
		OmImage<T, 2> slice = chunk.getSlice(plane, offset);
		return slice.recastToUint32().getMallocCopyOfData();
	}

	boost::shared_ptr<uint32_t> operator()(uint32_t* d ) const {
		OmImage<uint32_t, 3, OmImageRefData> chunk(OmExtents[128][128][128], d);
		OmImage<uint32_t, 2> slice = chunk.getSlice(plane, offset);
		return slice.getMallocCopyOfData();
	}

	boost::shared_ptr<uint32_t> operator()(float* d ) const {
		OmImage<float, 3, OmImageRefData> chunk(OmExtents[128][128][128], d);
		OmImage<float, 2> sliceFloat = chunk.getSlice(plane, offset);
		float mn = 0.0;
		float mx = 1.0;
		//	  mpMipVolume->GetBounds(mx, mn);
		OmImage<uint32_t, 2> slice =
			sliceFloat.rescaleAndCast<uint32_t>(mn, mx, 255.0);
		return slice.getMallocCopyOfData();
	}
private:
	const ViewType plane;
	const int offset;
};
boost::shared_ptr<uint32_t> OmChunkData::ExtractDataSlice32bit(const ViewType plane,
															   const int offset)
{
	return boost::apply_visitor(ExtractDataSlice32bitVisitor(plane, offset),
								getRawData());
}


class ProcessChunkVoxel {
public:
	ProcessChunkVoxel(OmMipChunk* chunk,
					  const bool computeSizes,
					  boost::shared_ptr<OmSegmentCache> segCache)
		: chunk_(chunk)
		, computeSizes_(computeSizes)
		, minVertexOfChunk_(chunk_->GetExtent().getMin())
		, segCache_(segCache)
	{}

	~ProcessChunkVoxel()
	{
		FOR_EACH(iter, sizes_){
			const OmSegID val = iter->first;
			OmSegment* seg = segCache_->GetOrAddSegment(val);
			seg->addToSize(sizes_[val]);
			seg->addToBounds(bounds_[val]);
		}
	}

	inline void processVoxel(const OmSegID val, const Vector3i& voxelPos)
	{
		chunk_->mDirectlyContainedValues.insert(val);

		if(!computeSizes_){
			return;
		}

		sizes_[val] = 1 + sizes_[val];
		bounds_[val].merge(DataBbox(minVertexOfChunk_ + voxelPos,
									minVertexOfChunk_ + voxelPos));
	}

private:
	OmMipChunk *const chunk_;
	const bool computeSizes_;
	const Vector3i minVertexOfChunk_;

	std::map<OmSegID, uint64_t> sizes_;
	std::map<OmSegID, DataBbox> bounds_;

	boost::shared_ptr<OmSegmentCache> segCache_;
};

class RefreshDirectDataValuesVisitor : public boost::static_visitor<>{
public:
	RefreshDirectDataValuesVisitor(OmMipChunk* chunk,
								   const bool computeSizes,
								   boost::shared_ptr<OmSegmentCache> segCache)
		: chunk_(chunk)
		, computeSizes_(computeSizes)
		, segCache_(segCache) {}

	template <typename T>
	void operator()(T* d ) const{
		doRefreshDirectDataValues(d);
	}
private:
	OmMipChunk *const chunk_;
	const bool computeSizes_;
	boost::shared_ptr<OmSegmentCache> segCache_;

	template <typename C>
	void doRefreshDirectDataValues(C* data) const
	{
		ProcessChunkVoxel p(chunk_, computeSizes_, segCache_);

		OmChunkVoxelWalker iter(128);
		for(iter.begin(); iter < iter.end(); ++iter){
			const OmSegID val = static_cast<OmSegID>(*data++);
			if(0 == val){
				continue;
			}
			p.processVoxel(val, *iter);
		}
	}
};
void OmChunkData::RefreshDirectDataValues(const bool computeSizes,
										  boost::shared_ptr<OmSegmentCache> segCache)
{
	boost::apply_visitor(RefreshDirectDataValuesVisitor(chunk_,
														computeSizes,
														segCache),
						 getRawData());
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
						 getRawData());
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
						 getRawData());
}
void OmChunkData::copyDataFromHDF5toMemMap(OmDataWrapperPtr hdf5)
{
	boost::apply_visitor(CopyDataFromHDF5toMemMapVisitor(chunk_, hdf5),
			     getRawData());
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
OmImage<uint32_t, 3> OmChunkData::GetCopyOfChunkDataAsOmImage32()
{
	return boost::apply_visitor(GetOmImage32ChunkVisitor(chunk_),
								getRawData());
}


class SetVoxelValueVisitor : public boost::static_visitor<uint32_t>{
public:
	SetVoxelValueVisitor(const DataCoord & voxel, uint32_t val)
		: voxel_(voxel), val_(val) {}

	template <typename T>
	uint32_t operator()(T* d) const {
		OmImage<T,3,OmImageRefData> data(OmExtents[128][128][128],
										 d);
		const uint32_t oldVal = data.getVoxel(voxel_.z, voxel_.y, voxel_.x);
		data.setVoxel(voxel_.z, voxel_.y, voxel_.x, val_);
		return oldVal;
	}
private:
	const DataCoord & voxel_;
	uint32_t val_;
};
uint32_t OmChunkData::SetVoxelValue(const DataCoord & voxel, uint32_t val)
{
	return boost::apply_visitor(SetVoxelValueVisitor(voxel, val),
								getRawData());
}


class GetVoxelValueVisitor : public boost::static_visitor<uint32_t>{
public:
	GetVoxelValueVisitor(const DataCoord & voxel)
		: voxel_(voxel) {}

	template <typename T>
	uint32_t operator()(T* d) const {
		OmImage<T,3,OmImageRefData> data(OmExtents[128][128][128],
										 d);
		return data.getVoxel(voxel_.z, voxel_.y, voxel_.x);
	}
private:
	const DataCoord & voxel_;
};
uint32_t OmChunkData::GetVoxelValue(const DataCoord & voxel)
{
	return boost::apply_visitor(GetVoxelValueVisitor(voxel),
								getRawData());
}


class CompareVisitor : public boost::static_visitor<bool>{
public:
	CompareVisitor(OmRawDataPtrs& other)
		: other_(other) {}

	template <typename T>
	bool operator()(T* d) const {
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
bool OmChunkData::compare(boost::shared_ptr<OmChunkData> other)
{
	return boost::apply_visitor(CompareVisitor(other->getRawData()),
								getRawData());
}

class GetMaxVisitor : public boost::static_visitor<double>{
public:
	template <typename T>
	double operator()(T* d) const {
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
	double operator()(T* d) const {
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
