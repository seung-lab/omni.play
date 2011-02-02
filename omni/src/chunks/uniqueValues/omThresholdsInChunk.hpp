#ifndef OM_CHUNK_UNIQUE_VALUES_HPP
#define OM_CHUNK_UNIQUE_VALUES_HPP

#include "chunks/uniqueValues/omChunkUniqueValuesPerThreshold.hpp"
#include "utility/fuzzyStdObjs.hpp"

class OmThresholdsInChunk {
private:
	OmSegmentation *const segmentation_;
	const OmChunkCoord coord_;
	zi::rwmutex lock_;

	DoubleFuzzyStdMap<boost::shared_ptr<OmChunkUniqueValuesPerThreshold> > valByThres_;

public:
	OmThresholdsInChunk(OmSegmentation* segmentation,
						const OmChunkCoord& coord)
		: segmentation_(segmentation)
		, coord_(coord)
	{}

	boost::shared_ptr<OmChunkUniqueValuesPerThreshold> Get(const double threshold)
	{
		zi::rwmutex::write_guard g(lock_);

		if(!valByThres_.count(threshold)){
			return valByThres_[threshold] =
				boost::make_shared<OmChunkUniqueValuesPerThreshold>(segmentation_,
																   coord_,
																   threshold);
		}
		return valByThres_[threshold];
	}
};

#endif
