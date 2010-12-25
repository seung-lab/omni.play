#ifndef OM_MEM_MAPPED_ALLOC_FILE_HPP
#define OM_MEM_MAPPED_ALLOC_FILE_HPP

#include "volume/omMipChunk.h"
#include "volume/omSegmentation.h"
#include "volume/omMipChunkCoord.h"
#include "mesh/io/v2/chunk/omMeshChunkTypes.h"

class OmMemMappedAllocFile {
private:
	OmSegmentation* segmentation_;
	const OmMipChunkCoord coord_;
	const double threshold_;
	const QString fnp_;

	boost::shared_ptr<QFile> file_;
	OmMeshDataEntry* table_;
	uint32_t numEntries_;

public:
	OmMemMappedAllocFile(OmSegmentation* segmentation,
						 const OmMipChunkCoord& coord)
		: segmentation_(segmentation)
		, coord_(coord)
		, threshold_(1.)
		, fnp_(filePath(threshold_))
	{}

	bool CreateIfNeeded()
	{
		QFile file(fnp_);
		if(file.exists()){
			return false;
		}

		setupFile();
		return true;
	}

	OmMeshDataEntry* Find(const OmMeshDataEntry& entry)
	{
		OmMeshDataEntry* iter =
			std::lower_bound(table_,
							 table_ + numEntries_,
							 entry,
							 compareBySegID);

		if(iter == table_ + numEntries_ ||
			iter->segID != entry.segID)
		{
			return NULL;
		}

		return iter;
	}

	void Unmap()
	{
		file_.reset();
		table_ = NULL;
		numEntries_ = 0;
	}

	inline bool MapIfNeeded()
	{
		if(file_){
			return false;
		}

		map();

		return true;
	}

private:

	void map()
	{
		file_ = boost::make_shared<QFile>(fnp_);
		if(!file_->exists()){
			throw OmIoException("file doesn't exist", fnp_);
		}

		if(!file_->open(QIODevice::ReadWrite)) {
			throw OmIoException("could not open", fnp_);
		}

		uchar* map = file_->map(0, file_->size());
		if(!map){
			throw OmIoException("could not map", fnp_);
		}

		file_->close();

		table_ = reinterpret_cast<OmMeshDataEntry*>(map);
		numEntries_ = file_->size() / sizeof(OmMeshDataEntry);
	}

	void setupFile()
	{
		OmMipChunkPtr chunk;
		segmentation_->GetChunk(chunk, coord_);

		const OmSegIDsSet& segIDs = chunk->GetUniqueSegIDs();

		file_ = boost::make_shared<QFile>(fnp_);
		if(!file_->open(QIODevice::ReadWrite)) {
			throw OmIoException("could not open", fnp_);
		}

		file_->resize(segIDs.size() * sizeof(OmMeshDataEntry));

		uchar* map = file_->map(0, file_->size());
		if(!map){
			throw OmIoException("could not map", fnp_);
		}

		file_->close();

		table_ = reinterpret_cast<OmMeshDataEntry*>(map);
		numEntries_ = segIDs.size();

		resetTable(segIDs);
		sortTable();

		std::cout << "in chunk " << coord_
				  << ", found " << numEntries_ << " segment IDs\n";
	}

	struct ResetEntry {
		OmMeshDataEntry operator()(const OmSegID segID) const {
			return om::meshio_::MakeEmptyEntry(segID);
		}
	};

	void resetTable(const OmSegIDsSet& segIDs)
	{
		zi::transform(segIDs.begin(),
					  segIDs.end(),
					  table_,
					  ResetEntry());
	}

	void sortTable()
	{
		zi::sort(table_,
				 table_ + numEntries_,
				 compareBySegID);
	}

	static bool compareBySegID(const OmMeshDataEntry& a,
							   const OmMeshDataEntry& b){
		return a.segID < b.segID;
	}

	QString filePath(const double threshold)
	{
		const QString volPath =
			OmFileNames::GetMeshChunkFolderPath(segmentation_,
												threshold, coord_);

		if(!QDir(volPath).exists()){
			OmFileNames::MakeMeshChunkFolderPath(segmentation_,
												 threshold, coord_);
		}

		const QString fullPath = QString("%1meshAllocTable.ver2")
			.arg(volPath);
		return fullPath;
	}
};

#endif
