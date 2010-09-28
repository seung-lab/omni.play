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
	DownsampleVisitor(const int level, OmMipVolume* vol,
					  OmRawDataPtrs prevLevelPtr)
		: level_(level)
		, prevLevel_(level-1)
		, vol_(vol)
		, prevLevelPtr_(prevLevelPtr)
	{}

	template <typename T>
	void operator()(T* dst) const {
		T* src = boost::get<T*>(prevLevelPtr_);
		assert(src);

		printf("\n************will downsample from %d to %d\n\n",
			   level_-1, level_);

		const Vector3i src_dims = vol_->getDimsRoundedToNearestChunk(prevLevel_);
		const Vector3i dest_dims = vol_->getDimsRoundedToNearestChunk(level_);

		const int64_t maxValidIndexSrc = src_dims.x * src_dims.y * src_dims.z - 1;
		const int64_t maxValidIndexDst = dest_dims.x * dest_dims.y * dest_dims.z - 1;

		std::cout << "downsampling level " << prevLevel_ << "(" << src_dims << ")"
				  << " to level " << level_ << "(" << dest_dims << ")\n";
		fflush(stdout);

		const int sliceSize = src_dims.x * src_dims.y;

		const Vector3<int64_t> chunkDims = vol_->GetChunkDimensions();

		for (int si=0,di=0,dz=0; dz < dest_dims.z; ++dz,si+=sliceSize){
			for (int dy=0; dy < dest_dims.y; ++dy, si+=src_dims.x){
				if(di < 2000){
					printf("src %d goes to dst %d\n", si, di);
				}
				for (int dx=0; dx < dest_dims.x; ++dx, ++di, si+=2){
					assert(di <= maxValidIndexDst);
					assert(si <= maxValidIndexSrc);
					dst[di] = src[si];
				}
			}
		}
	}

private:
	const int level_;
	const int prevLevel_;
	OmMipVolume* vol_;
	OmRawDataPtrs& prevLevelPtr_;
};

void OmVolumeData::downsample(OmMipVolume* vol)
{
	for (int level = 1; level <= vol->GetRootMipLevel(); ++level) {
		OmRawDataPtrs prevLevelVolPtr = GetVolPtr(level-1);
		OmRawDataPtrs levelVolPtr = GetVolPtr(level);

		boost::apply_visitor(DownsampleVisitor(level, vol, prevLevelVolPtr),
							 levelVolPtr);
	}
}
