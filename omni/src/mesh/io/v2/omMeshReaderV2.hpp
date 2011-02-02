#ifndef OM_MESH_READER_V2_HPP
#define OM_MESH_READER_V2_HPP

#include "common/omCommon.h"
#include "mesh/io/omDataForMeshLoad.hpp"
#include "mesh/io/v2/chunk/omMeshChunkDataReaderV2.hpp"
#include "mesh/io/v2/omMeshFilePtrCache.hpp"
#include "mesh/omMipMeshManager.h"

class OmMeshReaderV2{
private:
	OmSegmentation *const segmentation_;
	const double threshold_;
	OmMeshFilePtrCache *const filePtrCache_;

public:
	OmMeshReaderV2(OmSegmentation* segmentation, const double threshold)
		: segmentation_(segmentation)
		, threshold_(threshold)
		, filePtrCache_(segmentation->MeshManager(threshold_)->FilePtrCache())
	{}

	~OmMeshReaderV2()
	{}

	inline boost::shared_ptr<OmDataForMeshLoad>
	Read(const OmMipMeshCoord& meshCoord){
		return Read(meshCoord.SegID(), meshCoord.Coord());
	}

	boost::shared_ptr<OmDataForMeshLoad>
	Read(const OmSegID segID, const OmChunkCoord& coord)
	{
		OmMeshChunkAllocTableV2* chunk_table =
			filePtrCache_->GetAllocTable(coord);

		OmMeshChunkDataReaderV2 chunk_data(segmentation_, coord, threshold_);

		boost::shared_ptr<OmDataForMeshLoad> ret =
			boost::make_shared<OmDataForMeshLoad>();

		if(!chunk_table->Contains(segID)){
			return ret;
		}

		const OmMeshDataEntry entry = chunk_table->Find(segID);

		if(!entry.wasMeshed){
			std::cout << "did not yet mesh " << segID
					  << " in coord " << coord << "\n" << std::flush;
			throw OmIoException("mesh data not found");
		}

		ret->HasData(entry.hasMeshData);

		if(!entry.hasMeshData){
			return ret;
		}

		ret->SetVertexIndex(chunk_data.Read<uint32_t>(entry.vertexIndex),
							entry.vertexIndex.count,
							entry.vertexIndex.totalBytes);

		ret->SetStripData(chunk_data.Read<uint32_t>(entry.stripData),
						  entry.stripData.count,
						  entry.stripData.totalBytes);

		ret->SetVertexData(chunk_data.Read<float>(entry.vertexData),
						   entry.vertexData.count,
						   entry.vertexData.totalBytes);

		if(entry.trianData.totalBytes){
			ret->SetTrianData(chunk_data.Read<uint32_t>(entry.trianData),
							  entry.trianData.count,
							  entry.trianData.totalBytes);
		}

		return ret;
	}
};

#endif
