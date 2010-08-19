#include "volume/omVolumeData.hpp"

class AllocMemMapFilesVisitor : public boost::static_visitor<> {
public:
	AllocMemMapFilesVisitor(const std::map<int, Vector3i> & levDims)
		: levelsAndDims(levDims) {}
	template <typename T> void operator()( T & d ) const {
		d.AllocMemMapFiles(levelsAndDims);
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
		return d.getChunkPtr(coord);
	}
private:
	OmMipChunkCoord coord;
};
OmRawDataPtrs OmVolumeData::getChunkPtrRaw(const OmMipChunkCoord & coord){
	return boost::apply_visitor(GetChunkPtrVisitor(coord), volData_);
}


OmAllowedVolumeDataTypes OmVolumeData::determineOldVolType(OmMipVolume * vol)
{
	const OmMipChunkCoord coord(0,0,0,0);
	OmMipChunkPtr chunk;
	vol->GetChunk(chunk, coord);
	OmDataWrapperPtr data = chunk->RawReadChunkDataHDF5();
	return data->getVolDataType();
}
