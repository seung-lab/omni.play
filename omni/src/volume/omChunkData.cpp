#include "volume/omChunkData.hpp"
#include "volume/omVolumeData.hpp"

OmChunkData::OmChunkData(OmMipVolume* vol, const OmMipChunkCoord &coord)
	: rawData(vol->volData->getChunkPtrRaw(coord))
	, vol_(vol)
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

OmSegSizeMapPtr OmChunkData::RefreshDirectDataValues(OmMipChunk* chunk,
						     const bool computeSizes)
{
	return boost::apply_visitor(RefreshDirectDataValuesVisitor(chunk, computeSizes),
				    rawData);
}
