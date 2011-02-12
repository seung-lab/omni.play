#ifndef OM_SEG_CHUNK_DATA_VISITORS_HPP
#define OM_SEG_CHUNK_DATA_VISITORS_HPP

#include "chunks/omSegChunk.h"
#include "segment/omSegmentCache.h"
#include "utility/omChunkVoxelWalker.hpp"
#include "volume/build/omProcessSegmentationChunk.hpp"
#include "volume/io/omVolumeData.h"
#include "volume/omRawChunk.hpp"
#include "volume/omSegmentation.h"

class ExtractDataSlice32bitVisitor
	: public boost::static_visitor<boost::shared_ptr<uint32_t> >{
public:
	ExtractDataSlice32bitVisitor(const ViewType plane, int offset)
		: plane(plane), offset(offset) {}

	template <typename T>
	boost::shared_ptr<uint32_t> operator()(T* d) const
	{
		OmImage<T, 3, OmImageRefData> chunk(OmExtents[128][128][128], d);
		OmImage<T, 2> slice = chunk.getSlice(plane, offset);
		return slice.recastToUint32().getMallocCopyOfData();
	}

	boost::shared_ptr<uint32_t> operator()(uint32_t* d) const
	{
		OmImage<uint32_t, 3, OmImageRefData> chunk(OmExtents[128][128][128], d);
		OmImage<uint32_t, 2> slice = chunk.getSlice(plane, offset);
		return slice.getMallocCopyOfData();
	}

	boost::shared_ptr<uint32_t> operator()(float* d) const
	{
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

class RefreshDirectDataValuesVisitor : public boost::static_visitor<>{
public:
	RefreshDirectDataValuesVisitor(OmSegChunk* chunk,
								   const bool computeSizes,
								   OmSegmentCache* segCache)
		: chunk_(chunk)
		, computeSizes_(computeSizes)
		, segCache_(segCache) {}

	template <typename T>
	void operator()(T* d) const{
		doRefreshDirectDataValues(d);
	}

private:
	OmSegChunk *const chunk_;
	const bool computeSizes_;
	OmSegmentCache* segCache_;

	template <typename C>
	void doRefreshDirectDataValues(C* data) const
	{
		OmProcessSegmentationChunk p(chunk_, computeSizes_, segCache_);

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

class ProcessChunkVoxelBoundingData {
public:
	ProcessChunkVoxelBoundingData(OmSegChunk* chunk,
								  OmSegmentCache* segCache)
		: chunk_(chunk)
		, minVertexOfChunk_(chunk_->Mipping().GetExtent().getMin())
		, segCache_(segCache)
	{}

	~ProcessChunkVoxelBoundingData()
	{
		FOR_EACH(iter, localSegCache_){
			const OmSegID val = iter->first;
			OmSegment* seg = iter->second;
			seg->addToBounds(bounds_[val]);
		}
	}

	inline void processVoxel(const OmSegID val, const Vector3i& voxelPos)
	{
		getOrAddSegment(val);
		bounds_[val].merge(DataBbox(minVertexOfChunk_ + voxelPos,
									minVertexOfChunk_ + voxelPos));
	}

private:
	OmSegChunk *const chunk_;
	const Vector3i minVertexOfChunk_;
	OmSegmentCache *const segCache_;

	boost::unordered_map<OmSegID, OmSegment*> localSegCache_;
	boost::unordered_map<OmSegID, DataBbox> bounds_;

	OmSegment* getOrAddSegment(const OmSegID val)
	{
		if(!localSegCache_.count(val)){
			return localSegCache_[val] = segCache_->GetOrAddSegment(val);
		}
		return localSegCache_[val];
	}
};

class RefreshBoundingDataVisitor : public boost::static_visitor<>{
public:
	RefreshBoundingDataVisitor(OmSegChunk* chunk,
							   OmSegmentCache* segCache)
		: chunk_(chunk)
		, segCache_(segCache) {}

	template <typename T>
	void operator()(T* d) const{
		doRefreshBoundingData(d);
	}
private:
	OmSegChunk *const chunk_;
	OmSegmentCache* segCache_;

	template <typename C>
	void doRefreshBoundingData(C* data) const
	{
		ProcessChunkVoxelBoundingData p(chunk_, segCache_);

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

class SetVoxelValueVisitor : public boost::static_visitor<uint32_t>{
public:
	SetVoxelValueVisitor(const DataCoord & voxel, uint32_t val)
		: voxel_(voxel), val_(val) {}

	template <typename T>
	uint32_t operator()(T* d) const
	{
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

class RewriteChunkVisitor : public boost::static_visitor<>{
private:
	OmMipVolume *const vol_;
	OmSegChunk *const chunk_;
	const boost::unordered_map<uint32_t, uint32_t>& vals_;

public:
	RewriteChunkVisitor(OmMipVolume* vol, OmSegChunk* chunk,
						const boost::unordered_map<uint32_t, uint32_t>& vals)
		: vol_(vol)
		, chunk_(chunk)
		, vals_(vals)
	{}

	template <typename T>
	void operator()(T*) const
	{
		OmRawChunk<T> rawChunk(vol_, chunk_->GetCoordinate());
		rawChunk.SetDirty();
		T* d = rawChunk.Data();

		for(int i = 0; i < 128*128*128; ++i){
			d[i] = vals_.at(d[i]);
		}
	}
};

class GetOmImage32ChunkVisitor
	: public boost::static_visitor<boost::shared_ptr<uint32_t> > {
public:
	GetOmImage32ChunkVisitor(OmMipVolume* vol, OmSegChunk* chunk)
		: vol_(vol)
		, chunk_(chunk)
		, numVoxelsInChunk_(chunk_->Mipping().NumVoxels())
	{}

	template <typename T>
	boost::shared_ptr<uint32_t> operator()(T*) const
	{
		OmRawChunk<T> rawChunk(vol_, chunk_->GetCoordinate());

		boost::shared_ptr<T> data =	rawChunk.SharedPtr();
		T* dataRaw = data.get();

		boost::shared_ptr<uint32_t> ret =
			OmSmartPtr<uint32_t>::MallocNumElements(numVoxelsInChunk_,
													om::DONT_ZERO_FILL);
		std::copy(dataRaw,
				  dataRaw + numVoxelsInChunk_,
				  ret.get());

		return ret;
	}

	boost::shared_ptr<uint32_t> operator()(uint32_t*) const
	{
		OmRawChunk<uint32_t> rawChunk(vol_, chunk_->GetCoordinate());
		return rawChunk.SharedPtr();
	}

	boost::shared_ptr<uint32_t> operator()(float*) const {
		throw OmIoException("can't deal with float data!");
	}

private:
	OmMipVolume *const vol_;
	OmSegChunk *const chunk_;
	const int numVoxelsInChunk_;
};

#endif
