#ifndef OM_MESH_FILE_PTR_CACHE_HPP
#define OM_MESH_FILE_PTR_CACHE_HPP

#include "common/omCommon.h"
#include "mesh/io/v2/chunk/omMeshChunkAllocTable.hpp"
#include "mesh/io/v2/chunk/omMeshChunkDataReaderV2.hpp"
#include "mesh/io/v2/chunk/omMeshChunkDataWriterV2.hpp"
#include "mesh/io/v2/omRingBuffer.hpp"
#include "mesh/omMipMeshCoord.h"
#include "utility/omThreadPool.hpp"
#include "zi/omMutex.h"

class OmMipChunkCoord;
class OmMeshChunkAllocTableV2;
class OmMeshChunkDataWriterV2;

class OmMeshFilePtrCache {
private:
	OmSegmentation* segmentation_;

	std::map<OmMipChunkCoord, boost::shared_ptr<OmMeshChunkAllocTableV2> > tables_;
	std::map<OmMipChunkCoord, boost::shared_ptr<OmMeshChunkDataWriterV2> > data_;
	zi::rwmutex lock_;

	OmThreadPool threadPool_;

	OmRingBuffer<OmMeshChunkAllocTableV2> mappedFiles_;

public:
	OmMeshFilePtrCache(OmSegmentation* segmentation)
		: segmentation_(segmentation)
	{
		threadPool_.start();
	}

	~OmMeshFilePtrCache(){
		Stop();
	}

	void Join(){
		threadPool_.join();
	}

	void Stop(){
		threadPool_.stop();
	}

	void AddTaskBack(const boost::shared_ptr<zi::runnable> job){
		threadPool_.addTaskBack(job);
	}

	void RegisterMappedFile(OmMeshChunkAllocTableV2* table){
		mappedFiles_.Put(table);
	}

	OmMeshChunkAllocTableV2* GetAllocTable(const OmMipChunkCoord& coord)
	{
		zi::rwmutex::write_guard g(lock_);

		if(!tables_.count(coord)){
			tables_[coord] =
				boost::make_shared<OmMeshChunkAllocTableV2>(this, segmentation_, coord);
		}
		return tables_[coord].get();
	}

	OmMeshChunkDataWriterV2* GetWriter(const OmMipChunkCoord& coord)
	{
		zi::rwmutex::write_guard g(lock_);

		if(!data_.count(coord)){
			data_[coord] =
				boost::make_shared<OmMeshChunkDataWriterV2>(segmentation_, coord);
		}
		return data_[coord].get();
	}
};

#endif
