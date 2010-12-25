#ifndef OM_MESH_WRITER_V2_TASK_HPP
#define OM_MESH_WRITER_V2_TASK_HPP

#include "common/om.hpp"
#include "common/omCommon.h"
#include "mesh/detail/TriStripCollector.hpp"
#include "mesh/io/omDataForMeshLoad.hpp"
#include "mesh/io/v2/chunk/omMeshChunkAllocTable.hpp"
#include "mesh/io/v2/chunk/omMeshChunkDataWriterV2.hpp"
#include "mesh/io/v2/omMeshFilePtrCache.hpp"
#include "mesh/omMipMeshCoord.h"
#include "mesh/omMipMeshManager.h"
#include "zi/omThreads.h"

template <typename U>
class OmMeshWriterTaskV2 : public zi::runnable{
private:
	OmSegmentation *const seg_;
	OmMeshFilePtrCache *const  filePtrCache_;
	const OmSegID segID_;
	const OmMipChunkCoord coord_;
	const U mesh_;
	const om::AllowOverwrite allowOverwrite_;

public:
	OmMeshWriterTaskV2(OmSegmentation* seg,
					   OmMeshFilePtrCache* filePtrCache,
					   const OmSegID segID,
					   const OmMipChunkCoord& coord,
					   const U mesh,
					   const om::AllowOverwrite allowOverwrite)
		: seg_(seg)
		, filePtrCache_(filePtrCache)
		, segID_(segID)
		, coord_(coord)
		, mesh_(mesh)
		, allowOverwrite_(allowOverwrite)
	{}

	void run()
	{
		OmMeshChunkAllocTableV2* chunk_table =
			filePtrCache_->GetAllocTable(coord_);

		if(!chunk_table->Contains(segID_)){
			printInfoAboutSkippedSegment(mesh_);
			return;
		}

		// prevent ovlapping mesh saves; will have segid "lock"
		//  if returns true
		if(!chunk_table->CanContinueMeshSave(segID_)){
			return;
		}

		if(om::WRITE_ONCE == allowOverwrite_){
			const OmMeshDataEntry entry = chunk_table->Find(segID_);
			if(entry.wasMeshed){
				chunk_table->SegmentMeshSaveDone(segID_);
				return;
			}
		}

		OmMeshChunkDataWriterV2* chunk_data =
			filePtrCache_->GetWriter(coord_);

		const OmMeshDataEntry entry =
			writeOutData(chunk_data, mesh_, OmMipMeshCoord(coord_, segID_));

		chunk_table->Set(entry);

		chunk_table->SegmentMeshSaveDone(segID_);
	}

private:
	OmMeshDataEntry
	writeOutData(OmMeshChunkDataWriterV2* chunk_data,
				 boost::shared_ptr<OmDataForMeshLoad> data,
				 const OmMipMeshCoord& meshCoord)
	{
		OmMeshDataEntry entry =
			om::meshio_::MakeEmptyEntry(meshCoord.SegID());
		entry.wasMeshed = true;

		// "empty mesh"
		if(0 == data->VertexDataCount()){
			return entry;
		}

		entry.hasMeshData = true;

		chunk_data->Append<uint32_t>(meshCoord,
									 data->VertexIndexSharedPtr(),
									 entry.vertexIndex,
									 data->VertexIndexCount(),
									 data->VertexIndexNumBytes());

		chunk_data->Append<float>(meshCoord,
								  data->VertexDataSharedPtr(),
								  entry.vertexData,
								  data->VertexDataCount(),
								  data->VertexDataNumBytes());

		chunk_data->Append<uint32_t>(meshCoord,
									 data->StripDataSharedPtr(),
									 entry.stripData,
									 data->StripDataCount(),
									 data->StripDataNumBytes());

		if(data->TrianDataNumBytes()){
			chunk_data->Append<uint32_t>(meshCoord,
										 data->TrianDataSharedPtr(),
										 entry.trianData,
										 data->TrianDataCount(),
										 data->TrianDataNumBytes());
		}

		return entry;
	}

	OmMeshDataEntry
	writeOutData(OmMeshChunkDataWriterV2* chunk_data,
				 zi::shared_ptr<TriStripCollector> triStrips,
				 const OmMipMeshCoord& meshCoord)
	{
		std::vector<float>& data = triStrips->data_;
		std::vector<uint32_t>& indices = triStrips->indices_;
		std::vector<uint32_t>& strips = triStrips->strips_;

		OmMeshDataEntry entry =
			om::meshio_::MakeEmptyEntry(meshCoord.SegID());
		entry.wasMeshed = true;

		// "empty mesh"
		if(0 == indices.size()){
			return entry;
		}

		entry.hasMeshData = true;

		chunk_data->Append<uint32_t>(meshCoord,
									 indices,
									 entry.vertexIndex,
									 indices.size());

		chunk_data->Append<float>(meshCoord,
								  data,
								  entry.vertexData,
								  data.size() / 6);

		chunk_data->Append<uint32_t>(meshCoord,
									 strips,
									 entry.stripData,
									 strips.size() / 2);

		triStrips->clear();

		return entry;
	}

	void printInfoAboutSkippedSegment(boost::shared_ptr<OmDataForMeshLoad>)
	{
		return;

		std::cout << "skipping segID " << segID_
				  << " in chunk " << coord_
				  << "\n";
	}

	void printInfoAboutSkippedSegment(zi::shared_ptr<TriStripCollector> triStrips)
	{
		return;

		printf(".");

		const int vertexData = triStrips->data_.size();
		const int vertexIndex = triStrips->indices_.size();
		const int stripsSize = triStrips->strips_.size();

		std::cout << "skipping segID " << segID_
				  << " in chunk " << coord_
				  << "; vertexData size (" << vertexData << ")"
				  << "; vertexIndex size (" << vertexIndex << ")"
				  << "; stripsSize size (" << stripsSize << ")"
				  << "\n";
	}

};

#endif
