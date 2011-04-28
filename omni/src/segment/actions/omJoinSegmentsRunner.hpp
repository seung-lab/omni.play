#ifndef OM_JOIN_SEGMENTS_RUNNER_HPP
#define OM_JOIN_SEGMENTS_RUNNER_HPP

#include "utility/segmentationDataWrapper.hpp"
#include "segment/omSegmentUtils.hpp"
#include "segment/actions/omSetSegmentValid.hpp"

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

    void Join()
    {
        const bool joiningValidatedSegments = sdw_.Segments()->AreAnySegmentsInValidList(ids_);

        if(joiningValidatedSegments){
            joinValidatedSegments();

        } else {
            (new OmSegmentJoinAction(sdw_, ids_))->Run();
        }
    }

private:
    /*
     * if the join involves validated segments:
     *  1.) find all child segments across all segments involved in the join
     *  2.) invalidate all segments
     *  3.) perform the join
     *  4.) (re)validate all segments
     */
    void joinValidatedSegments()
    {
        std::deque<OmSegment*> segPtrs;
        OmSegmentUtils::GetAllChildrenSegments(sdw_, ids_, segPtrs);

        OmSetSegmentValid validator(sdw_);
        validator.SetAsNotValidForJoin(segPtrs);

        (new OmSegmentJoinAction(sdw_, ids_))->Run();

        validator.SetAsValidForJoin(segPtrs);
    }
};

#endif
