#ifndef OM_SEGMENT_JOIN_ACTION_IMPL_HPP
#define OM_SEGMENT_JOIN_ACTION_IMPL_HPP

#include "common/omCommon.h"
#include "segment/actions/omJoinSegments.hpp"

class OmSegmentJoinActionImpl {
private:
    SegmentationDataWrapper sdw_;
    OmSegIDsSet segIDs_;

public:
    OmSegmentJoinActionImpl()
    {}

    OmSegmentJoinActionImpl(const SegmentationDataWrapper& sdw,
                            const OmSegIDsSet& segIDs)
        : sdw_(sdw)
        , segIDs_(segIDs)
    {}

    void Execute()
    {
        OmJoinSegments joiner(sdw_, segIDs_);
        segIDs_ = joiner.Join();
    }

    void Undo()
    {
        OmJoinSegments joiner(sdw_, segIDs_);
        segIDs_ = joiner.UnJoin();
    }

    std::string Description()
    {
        if(segIDs_.empty()){
            return "did not join segments";
        }

        static const int max = 5;

        const std::string nums =
            om::utils::MakeShortStrList<OmSegIDsSet, OmSegID>(segIDs_, max);

        return "Joined segments: " + nums;
    }

    QString classNameForLogFile() const {
        return "OmSegmentJoinAction";
    }

private:
    template <typename T> friend class OmActionLoggerThread;

    friend class QDataStream &operator<<(QDataStream&, const OmSegmentJoinActionImpl&);
    friend class QDataStream &operator>>(QDataStream&, OmSegmentJoinActionImpl&);

    friend QTextStream& operator<<(QTextStream&, const OmSegmentJoinActionImpl&);
};

#endif
