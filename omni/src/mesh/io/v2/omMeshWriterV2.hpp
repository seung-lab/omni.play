#ifndef OM_MESH_IO_V2_HPP
#define OM_MESH_IO_V2_HPP

#include "common/omCommon.h"
#include "mesh/detail/TriStripCollector.hpp"
#include "mesh/io/omDataForMeshLoad.hpp"
#include "mesh/io/v2/chunk/omMeshChunkAllocTable.hpp"
#include "mesh/io/v2/chunk/omMeshChunkDataWriterV2.hpp"
#include "mesh/io/v2/threads/omMeshWriterTaskV2.hpp"
#include "mesh/io/v2/omMeshFilePtrCache.hpp"
#include "mesh/omMipMeshCoord.h"
#include "mesh/omMipMeshManager.h"

class OmMeshWriterV2{
private:
	OmSegmentation* seg_;
	OmMeshFilePtrCache* filePtrCache_;

public:
	OmMeshWriterV2(OmSegmentation* seg)
		: seg_(seg)
		, filePtrCache_(seg->MeshManager()->FilePtrCache())
	{}

	~OmMeshWriterV2(){
		Stop();
	}

	void Join(){
		filePtrCache_->Join();
	}

	void Stop(){
		filePtrCache_->Stop();
	}

	bool Contains(const OmSegID segID, const OmMipChunkCoord& coord)
	{
		OmMeshChunkAllocTableV2* chunk_table =
			filePtrCache_->GetAllocTable(coord);
		return chunk_table->Contains(segID);
	}

	bool WasMeshed(const OmSegID segID, const OmMipChunkCoord& coord)
	{
		OmMeshChunkAllocTableV2* chunk_table =
			filePtrCache_->GetAllocTable(coord);

		if(!chunk_table->Contains(segID)){
			throw OmIoException("segID not present");
		}

		const OmMeshDataEntry entry = chunk_table->Find(segID);

		return entry.wasMeshed;
	}

	bool HasData(const OmSegID segID, const OmMipChunkCoord& coord)
	{
		OmMeshChunkAllocTableV2* chunk_table =
			filePtrCache_->GetAllocTable(coord);

		if(!chunk_table->Contains(segID)){
			throw OmIoException("segID not present");
		}

		const OmMeshDataEntry entry = chunk_table->Find(segID);

		if(!entry.wasMeshed){
			throw OmIoException("was not yet meshed");
		}

		return entry.hasMeshData;
	}

	// Save will take ownership of mesh data
	template <typename U>
	void Save(const OmSegID segID, const OmMipChunkCoord& coord,
			  const U data, const om::ShouldBufferWrites buffferWrites,
			  const om::AllowOverwrite allowOverwrite)
	{
		boost::shared_ptr<OmMeshWriterTaskV2<U> > task =
			boost::make_shared<OmMeshWriterTaskV2<U> >(seg_,
													   filePtrCache_,
													   segID,
													   coord,
													   data,
													   allowOverwrite);

		if(om::BUFFER_WRITES == buffferWrites){
			filePtrCache_->AddTaskBack(task);
		} else {
			task->run();
		}
	}
};

#endif
