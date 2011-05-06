#ifndef OM_JOIN_SEGMENTS_RUNNER_HPP
#define OM_JOIN_SEGMENTS_RUNNER_HPP

#include "utility/segmentationDataWrapper.hpp"

class OmJoinSegmentsRunner {
private:
    SegmentationDataWrapper sdw_;
    OmSegIDsSet ids_;

public:
    OmJoinSegmentsRunner(const OmID segmentationID)
        : sdw_(segmentationID)
    {
        if(sdw_.IsSegmentationValid()){
            ids_ = sdw_.Segments()->GetSelectedSegmentIds();
        }
    }

    OmJoinSegmentsRunner(const SegmentationDataWrapper& sdw)
        : sdw_(sdw)
    {
        if(sdw_.IsSegmentationValid()){
            ids_ = sdw_.Segments()->GetSelectedSegmentIds();
        }
    }

    OmJoinSegmentsRunner(const OmID segmentationID, const OmSegIDsSet& ids)
        : sdw_(segmentationID)
        , ids_(ids)
    {}

    void Join(){
        (new OmSegmentJoinAction(sdw_, ids_))->Run();
    }
};

#endif
