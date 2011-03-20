#ifndef OM_MESH_CHUNK_ALLOC_TABLE_V2_HPP
#define OM_MESH_CHUNK_ALLOC_TABLE_V2_HPP

#include "datalayer/fs/omFileNames.hpp"
#include "mesh/io/v2/chunk/omMemMappedAllocFile.hpp"
#include "utility/omLockedObjects.h"

class OmMeshFilePtrCache;

class OmMeshChunkAllocTableV2 {
private:
	OmMeshFilePtrCache *const filePtrCache_;
	boost::shared_ptr<OmMemMappedAllocFile> file_;

	LockedBoostSet<OmSegID> segsBeingSaved_;
	zi::rwmutex lock_;

public:
	OmMeshChunkAllocTableV2(OmMeshFilePtrCache* filePtrCache,
							OmSegmentation* seg,
							const OmChunkCoord& coord,
							const double threshold)
		: filePtrCache_(filePtrCache)
		, file_(boost::make_shared<OmMemMappedAllocFile>(seg, coord, threshold))
	{
		zi::rwmutex::write_guard g(lock_);

		if(file_->CreateIfNeeded()){
			registerMappedFile();
		}
	}

	~OmMeshChunkAllocTableV2()
	{}

    // avoid over-lapping writes for meshes with same segment ID
	bool CanContinueMeshSave(const OmSegID segID)
	{
		if(segsBeingSaved_.insertSinceWasntPresent(segID)){
			return true;
		}
		printf("mesh already being saved...\n");
		return false;
	}

	void SegmentMeshSaveDone(const OmSegID segID){
		segsBeingSaved_.erase(segID);
	}

	void Unmap()
	{
		zi::rwmutex::write_guard g(lock_);
		file_->Unmap();
	}

	bool Contains(const OmMeshDataEntry& newEntry)
	{
		zi::rwmutex::write_guard g(lock_);

		map();

		OmMeshDataEntry* entry = file_->Find(newEntry);
		if(!entry){
			return false;
		}
		return true;
	}

	inline bool Contains(const OmSegID segID)
	{
		OmMeshDataEntry e;
		e.segID = segID;
		return Contains(e);
	}

	void Set(const OmMeshDataEntry& newEntry)
	{
		zi::rwmutex::write_guard g(lock_);

		map();

		OmMeshDataEntry* entry = file_->Find(newEntry);
		if(!entry){
			throw OmIoException("unknown segment ID");
		}

		assert(newEntry.segID == entry->segID);

		(*entry) = newEntry;
	}

	const OmMeshDataEntry Find(const OmSegID segID)
	{
		zi::rwmutex::write_guard g(lock_);

		map();

		OmMeshDataEntry e;
		e.segID = segID;

		OmMeshDataEntry* entry = file_->Find(e);
		if(!entry){
			throw OmIoException("unknown segment ID");
		}

		return *entry;
	}

private:
	inline void map()
	{
		if(file_->MapIfNeeded()){
			registerMappedFile();
		}
	}

	void registerMappedFile();
};

#endif
