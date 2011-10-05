#pragma once

#include "chunks/uniqueValues/chunkUniqueValuesPerThreshold.hpp"
#include "utility/fuzzyStdObjs.hpp"

class thresholdsInChunk {
private:
    segmentation *const segmentation_;
    const om::chunkCoord coord_;

    typedef DoubleFuzzyStdMap<chunkUniqueValuesPerThreshold*> map_t;
    typedef map_t::iterator iterator;
    typedef map_t::value_type value_t;

    map_t valByThres_;
    zi::mutex lock_;

public:
    thresholdsInChunk(segmentation* segmentation,
                        const om::chunkCoord& coord)
        : segmentation_(segmentation)
        , coord_(coord)
    {}

    ~thresholdsInChunk()
    {
        zi::guard g(lock_);
        FOR_EACH(iter, valByThres_){
            delete iter->second;
        }
    }

    chunkUniqueValuesPerThreshold* Get(const double threshold)
    {
        zi::guard g(lock_);

        iterator iter = valByThres_.lower_bound(threshold);

        if(iter != valByThres_.end() && !(valByThres_.key_comp()(threshold, iter->first)))
        {
            return iter->second;
        }

        iterator p = valByThres_.insert(iter,
                                        value_t(threshold,
                                                new chunkUniqueValuesPerThreshold(segmentation_,
                                                                                    coord_,
                                                                                    threshold)));
        return p->second;
    }
};

