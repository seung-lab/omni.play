#ifndef OM_MESH_CHUNK_ALLOC_TABLE_V2_HPP
#define OM_MESH_CHUNK_ALLOC_TABLE_V2_HPP

#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/fs/omVecInFile.hpp"
#include "mesh/io/v2/omMeshChunkTypes.h"
#include "utility/omLockedPODs.hpp"
#include "volume/omMipChunk.h"
#include "volume/omMipChunkCoord.h"

class OmMeshChunkAllocTableV2 {
private:
	OmSegmentation* seg_;
	const OmMipChunkPtr& chunk_;
	const OmMipChunkCoord& coord_;

	boost::shared_ptr<OmVectorInFile<OmMeshDataEntry> > file_;
	boost::optional<std::vector<OmMeshDataEntry>& > table_;
	LockedBool dirty_;

public:
	OmMeshChunkAllocTableV2(OmSegmentation* seg,
							OmMipChunkPtr& chunk)
		: seg_(seg)
		, chunk_(chunk)
		, coord_(chunk_->GetCoordinate())
	{
		dirty_.set(false);

		file_ =	boost::make_shared<OmVectorInFile<OmMeshDataEntry> >(filePath());
		if(file_->Exists()){
			file_->Load();
			table_ = file_->Vector();
		}else {
			setupFile();
		}
	}

	~OmMeshChunkAllocTableV2()
	{
		if(dirty_.get()){
			Save();
		}
	}

	void Load(){
		file_->Load();
	}

	void Save(){
		file_->Save();
	}

	void Set(const OmMeshDataEntry& newEntry)
	{
		OmMeshDataEntry& entry = findEntry(newEntry);
		entry = newEntry;
		dirty_.set(true);
	}

	const OmMeshDataEntry& Get(const OmSegID segID) const
	{
		OmMeshDataEntry e;
		e.segID = segID;
		return findEntry(e);
	}

private:
	static bool compareBySegID(const OmMeshDataEntry& a,
							   const OmMeshDataEntry& b){
		return a.segID < b.segID;
	}

	OmMeshDataEntry& findEntry(const OmMeshDataEntry& entry) const
	{
		std::vector<OmMeshDataEntry>::iterator iter
			= std::lower_bound(table_->begin(),
							   table_->end(),
							   entry,
							   compareBySegID);

		if(iter == table_->end()){
			throw OmIoException("could not find segment");
		}

		return *iter;
	}

	void setupFile()
	{
		const OmSegIDsSet& segIDs = chunk_->GetDirectDataValues();

		table_ = file_->Vector();
		table_->resize(segIDs.size());

		int i = 0;
		FOR_EACH(iter, segIDs){
			(*table_)[i++].segID = *iter;
		}

		zi::sort(table_->begin(), table_->end(), compareBySegID);

		dirty_.set(true);
	}

	QString filePath()
	{
		const QString volPath =
			OmFileNames::MakeVolMeshesPath(seg_, 1., coord_);
		const QString fullPath = QString("%1meshAllocTable.ver1")
			.arg(volPath);
		return fullPath;
	}

	friend class OmMeshChunkAllocTableTests;
};

#endif
