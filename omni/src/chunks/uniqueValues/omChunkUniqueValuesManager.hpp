#ifndef OM_CHUNK_UNIQUE_VALUES_MANAGER_HPP
#define OM_CHUNK_UNIQUE_VALUES_MANAGER_HPP

#include "chunks/uniqueValues/omChunkUniqueValuesPerThreshold.hpp"
#include "chunks/uniqueValues/omThresholdsInChunk.hpp"

class OmChunkUniqueValuesManager {
private:
	OmSegmentation *const segmentation_;
	zi::rwmutex lock_;

	std::map<OmChunkCoord, boost::shared_ptr<OmThresholdsInChunk> > chunks_;

public:
	OmChunkUniqueValuesManager(OmSegmentation* segmentation)
		: segmentation_(segmentation)
	{}

	ChunkUniqueValues Values(const OmChunkCoord& coord,
							 const double threshold)
	{
		boost::shared_ptr<OmThresholdsInChunk> chunk = get(coord);
		boost::shared_ptr<OmChunkUniqueValuesPerThreshold> chunkPerThres =
			chunk->Get(threshold);

		return chunkPerThres->Values();
	}

	void Clear()
	{
		zi::rwmutex::write_guard g(lock_);
		chunks_.clear();
	}

private:
	boost::shared_ptr<OmThresholdsInChunk> get(const OmChunkCoord& coord)
	{
		zi::rwmutex::write_guard g(lock_);

		if(!chunks_.count(coord)){
			return chunks_[coord] =
				boost::make_shared<OmThresholdsInChunk>(segmentation_,
														 coord);
		}
		return chunks_[coord];
	}
};

#endif
