#pragma once

#include "common/common.h"
#include "segment/segment.h"
#include "utility/lockedPODs.hpp"

#include "boost/unordered_map.hpp"

namespace om {
namespace volume {
class segmentation;
class SegmentationDataWrapper;
}

namespace segment {

class segments;
class segmentsStore;

class segmentsImplLowLevel {
public:
    segmentsImplLowLevel(volume::segmentation*, segmentsStore*);
    virtual ~segmentsImplLowLevel();

    void growGraphIfNeeded(segment* newSeg);

    inline common::segId GetNumSegments() const {
        return mNumSegs;
    }

    void SetNumSegments(const uint32_t num){
        mNumSegs = num;
    }

    std::string getSegmentName(common::segId segID);
    void setSegmentName(common::segId segID, std::string name);

    std::string getSegmentNote(common::segId segID);
    void setSegmentNote(common::segId segID, std::string note);

    inline uint32_t getMaxValue() const {
        return maxValue_.get();
    }

    volume::SegmentationDataWrapper GetSDW() const;

    segmentsStore* SegmentStore(){
        return store_;
    }

protected:
    volume::segmentation *const segmentation_;
    segmentsStore *const store_;

    utility::lockedUint32 maxValue_;
    uint32_t mNumSegs;

    typedef boost::unordered_map<common::id, std::string>::iterator map_it;
    boost::unordered_map< common::id, std::string > segmentCustomNames;
    boost::unordered_map< common::id, std::string > segmentNotes;

    inline common::segId getNextValue(){
        return maxValue_.inc();
    }

private:
    friend class segmentSelection;
    friend class enabledSegments;
    friend class SegmentTests;
};

} // namespace segment
} // namespace om
