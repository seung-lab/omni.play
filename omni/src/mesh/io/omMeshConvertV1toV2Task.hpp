#ifndef OM_MESH_CONVERT_V1_TO_V2_TASK_HPP
#define OM_MESH_CONVERT_V1_TO_V2_TASK_HPP

#include "mesh/io/v1/omMeshReaderV1.hpp"
#include "mesh/io/v2/omMeshWriterV2.hpp"
#include "mesh/io/omMeshMetadata.hpp"
#include "system/cache/omCacheManager.h"
#include "zi/omThreads.h"

class OmMeshConvertV1toV2Task : public zi::runnable{
private:
	OmSegmentation *const segmentation_;
	const double threshold_;

	boost::shared_ptr<OmMeshReaderV1> hdf5Reader_;
	boost::shared_ptr<OmMeshWriterV2> meshWriter_;

public:
	OmMeshConvertV1toV2Task(OmSegmentation* segmentation)
		: segmentation_(segmentation)
		, threshold_(1)
		, hdf5Reader_(new OmMeshReaderV1(segmentation_))
		, meshWriter_(new OmMeshWriterV2(segmentation_, threshold_))
	{}

	void run()
	{
		printf("copying mesh data...\n");

		boost::shared_ptr<std::deque<OmChunkCoord> > coordsPtr =
			segmentation_->GetMipChunkCoords();

		FOR_EACH(iter, *coordsPtr){
			const OmChunkCoord& coord = *iter;

			if(!processChunk(coord)){
				return;
			}
		}

		meshWriter_->Join();

		segmentation_->MeshManager(threshold_)->Metadata()->SetMeshedAndStorageAsChunkFiles();

		printf("mesh conversion done!\n");
	}

private:
	bool processChunk(const OmChunkCoord& coord)
	{
		const ChunkUniqueValues segIDs =
			segmentation_->ChunkUniqueValues()->Values(coord, 1);

		FOR_EACH(segID, segIDs){

			if(OmCacheManager::AmClosingDown()){
				return false;
			}

			if(meshWriter_->WasMeshed(*segID, coord)){
				continue;
			}

			boost::shared_ptr<OmDataForMeshLoad> mesh =
				hdf5Reader_->Read(*segID, coord);

			meshWriter_->Save(*segID, coord, mesh, om::DONT_BUFFER_WRITES,
							  om::WRITE_ONCE);

			//std::cout << "migrated segID " << *segID
					  //<< " coord " << coord << "\n";
		}

		return true;
	}
};

#endif
