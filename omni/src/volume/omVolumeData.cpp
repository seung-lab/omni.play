#include "volume/omVolumeData.hpp"
#include "volume/omMipChunk.h"

class LoadMemMapFilesVisitor : public boost::static_visitor<> {
public:
	template <typename T> void operator()( T & d ) const {
		d.Load();
	}
};
void OmVolumeData::loadMemMapFiles()
{
	boost::apply_visitor(LoadMemMapFilesVisitor(), volData_);
}

class AllocMemMapFilesVisitor : public boost::static_visitor<> {
public:
	AllocMemMapFilesVisitor(const std::map<int, Vector3i> & levDims)
		: levelsAndDims(levDims) {}
	template <typename T> void operator()( T & d ) const {
		d.Create(levelsAndDims);
	}
private:
	const std::map<int, Vector3i> levelsAndDims;
};
void OmVolumeData::allocMemMapFiles(const std::map<int, Vector3i> & levDims){
	boost::apply_visitor(AllocMemMapFilesVisitor(levDims), volData_);
}


class GetBytesPerSampleVisitor : public boost::static_visitor<int> {
public:
	template <typename T> int operator()( T & d ) const {
		return d.GetBytesPerSample();
	}
};
int OmVolumeData::GetBytesPerSample(){
	return boost::apply_visitor(GetBytesPerSampleVisitor(), volData_);
}


class GetChunkPtrVisitor : public boost::static_visitor<OmRawDataPtrs>{
public:
	GetChunkPtrVisitor(const OmMipChunkCoord & coord)
		: coord(coord) {}

	template <typename T>
	OmRawDataPtrs operator()(T & d ) const {
		return d.GetChunkPtr(coord);
	}
private:
	OmMipChunkCoord coord;
};
OmRawDataPtrs OmVolumeData::getChunkPtrRaw(const OmMipChunkCoord& coord){
	return boost::apply_visitor(GetChunkPtrVisitor(coord), volData_);
}


OmVolDataType OmVolumeData::determineOldVolType(OmMipVolume* vol)
{
	const OmMipChunkCoord coord(0,0,0,0);
	OmMipChunkPtr chunk;
	vol->GetChunk(chunk, coord);
	OmDataWrapperPtr data = chunk->RawReadChunkDataHDF5();
	return data->getVolDataType();
}


class GetVolPtrVisitor : public boost::static_visitor<OmRawDataPtrs>{
public:
	GetVolPtrVisitor(const int level) : level_(level) {}

	template <typename T>
	OmRawDataPtrs operator()(T & d ) const {
		return d.GetPtr(level_);
	}
private:
	const int level_;
};
OmRawDataPtrs OmVolumeData::GetVolPtr(const int level)
{
	return boost::apply_visitor(GetVolPtrVisitor(level), volData_);
}


class DownsampleVisitor : public boost::static_visitor<>{
public:
	DownsampleVisitor(OmMipVolume* vol)
		: vol_(vol)
	{}

	template <typename T>
	void operator()(T* src) const
	{


		const Vector3<uint64_t> src_dims = vol_->getDimsRoundedToNearestChunk(0);
		std::vector<T*> mips(vol_->GetRootMipLevel() + 1);
		for(int i=0; i <= vol_->GetRootMipLevel(); ++i){
			mips[i] = boost::get<T*>(vol_->getVolData()->GetVolPtr(i));
			assert(mips[i]);
		}

		for(uint64_t sz=0; sz < src_dims.z; sz+=2){
			for(uint64_t sy=0; sy < src_dims.y; sy+=2){
				for(uint64_t sx=0; sx < src_dims.x; sx+=2){

				}
			}
		}
	}

private:
	OmMipVolume* vol_;
};

void OmVolumeData::downsample(OmMipVolume* vol)
{
	OmRawDataPtrs data = GetVolPtr(0);
	boost::apply_visitor(DownsampleVisitor(vol),
						 data);
}
