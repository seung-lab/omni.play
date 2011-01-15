#ifndef OM_MESH_READER_V1_HPP
#define OM_MESH_READER_V1_HPP

#include "common/omDebug.h"
#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omIDataReader.h"
#include "datalayer/omIDataWriter.h"
#include "mesh/io/omDataForMeshLoad.hpp"
#include "mesh/omMipMeshCoord.h"
#include "volume/omMipChunk.h"
#include "volume/omSegmentation.h"
#include "system/omProjectData.h"

class OmMeshReaderV1{
private:
	OmSegmentation* segmentation_;

public:
	OmMeshReaderV1(OmSegmentation* segmentation)
		: segmentation_(segmentation)
	{}

	bool IsAnyMeshDataPresent()
	{
		const OmMipChunkCoord coord(0, 0, 0, 0);

		OmMipChunkPtr chunk;
		segmentation_->GetChunk(chunk, coord);
		const OmSegIDsSet& segIDs = chunk->GetUniqueSegIDs();

		FOR_EACH(segID, segIDs){
			if(isMeshDataPresent(*segID, coord)){
				return true;
			}
		}

		return false;
	}

	inline boost::shared_ptr<OmDataForMeshLoad>
	Read(const OmMipMeshCoord& meshCoord){
		return Read(meshCoord.SegID(), meshCoord.Coord());
	}

	inline boost::shared_ptr<OmDataForMeshLoad>
	Read(const OmSegID segID, const OmMipChunkCoord& coord)
	{
		try{
			return doRead(segID, coord);
		} catch(...){
			return boost::make_shared<OmDataForMeshLoad>();
		}
	}

private:

 	bool isMeshDataPresent(const OmSegID segID, const OmMipChunkCoord& coord)
	{
		try{
			return doIsMeshDataPresent(segID, coord);
		} catch(...){
			return false;
		}
	}

 	bool doIsMeshDataPresent(const OmSegID segID, const OmMipChunkCoord& coord)
	{
		const OmMipMeshCoord meshCoord(coord, segID);

		const std::string path = getDirectoryPath(meshCoord);

		OmDataPath fpath( path + "metamesh.dat" );
		if( !OmProjectData::GetProjectIDataReader()->dataset_exists( fpath ) ){
			return false;
		}

		OmDataWrapperPtr result = OmProjectData::GetProjectIDataReader()->readDataset(fpath);
		unsigned char noData = *(result->getPtr<unsigned char>());
		if ( 0 == noData ){
			return false;
		}

		return true;
	}

	boost::shared_ptr<OmDataForMeshLoad>
	doRead(const OmSegID segID, const OmMipChunkCoord& coord)
	{
		boost::shared_ptr<OmDataForMeshLoad> ret =
			boost::make_shared<OmDataForMeshLoad>();

		const OmMipMeshCoord meshCoord(coord, segID);

		const std::string path = getDirectoryPath(meshCoord);

		OmDataPath fpath( path + "metamesh.dat" );
		if( !OmProjectData::GetProjectIDataReader()->dataset_exists( fpath ) ){
			return ret;
		}

		OmDataWrapperPtr result = OmProjectData::GetProjectIDataReader()->readDataset(fpath);
		unsigned char noData = *(result->getPtr<unsigned char>());
		if ( 0 == noData ){
			return ret;
		}

		ret->HasData(true);

		std::pair<int, boost::shared_ptr<uint32_t> > trian =
			copyOutData<uint32_t>(path, "trianoffset.dat" );
		if(trian.first){
			ret->SetTrianData(trian.second,
							  trian.first / (2 * sizeof(uint32_t)),
							  trian.first);
		}

		std::pair<int, boost::shared_ptr<uint32_t> > strips =
			copyOutData<uint32_t>(path, "stripoffset.dat" );
		if(strips.first){
			ret->SetStripData(strips.second,
							  strips.first / (2 * sizeof(uint32_t)),
							  strips.first);
		}

		std::pair<int, boost::shared_ptr<uint32_t> > vertexIndex =
			copyOutData<uint32_t>(path, "vertexoffset.dat" );
		if(vertexIndex.first){
			ret->SetVertexIndex(vertexIndex.second,
								vertexIndex.first / sizeof(uint32_t),
								vertexIndex.first);
		}

		std::pair<int, boost::shared_ptr<float> > vertexData =
			copyOutData<float>(path, "vertex.dat" );
		if(vertexData.first){
			ret->SetVertexData(vertexData.second,
							   vertexData.first / (6 * sizeof(float)),
							   vertexData.first);
		}

		return ret;
	}

	std::string getDirectoryPath(const OmMipMeshCoord& meshCoord)
	{
		const std::string p = str( boost::format("%1%/%2%_%3%_%4%/mesh/%5%/")
								   % meshCoord.MipChunkCoord.Level
								   % meshCoord.MipChunkCoord.Coordinate.x
								   % meshCoord.MipChunkCoord.Coordinate.y
								   % meshCoord.MipChunkCoord.Coordinate.z
								   % meshCoord.DataValue);

		return segmentation_->GetDirectoryPath() + p;
	}

	template <typename T>
	std::pair<int, boost::shared_ptr<T> >
	copyOutData(const std::string& path, const std::string& fileName)
	{
		const OmDataPath fpath( path + fileName );

		int size;
		boost::shared_ptr<OmDataWrapperBase> dataPtr =
			OmProjectData::GetProjectIDataReader()->readDataset(fpath, &size);

		boost::shared_ptr<T> data;

		if(!size){
			return std::make_pair(size, data);
		}

		OmDataWrapper<T>* dataPtrReint =
			reinterpret_cast<OmDataWrapper<T>*>(dataPtr.get());

		data = dataPtrReint->Ptr();

		return std::make_pair(size, data);
	}
};

#endif
