#ifndef OM_SEGMENTATION_THRESHOLD_CHANGE_IMPL_HPP
#define OM_SEGMENTATION_THRESHOLD_CHANGE_IMPL_HPP

#include "actions/io/omActionLogger.hpp"
#include "common/omCommon.h"
#include "common/omString.hpp"
#include "system/events/omSegmentEvent.h"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"

class OmSegmentationThresholdChangeActionImpl {
private:
    OmID mSegmentationId;
    float mThreshold;
    float mOldThreshold;

public:
    OmSegmentationThresholdChangeActionImpl()
    {}

    OmSegmentationThresholdChangeActionImpl(const OmID segmentationId,
                                            const float threshold)
        : mSegmentationId( segmentationId )
        , mThreshold( threshold )
    {}

    void Execute()
    {
        SegmentationDataWrapper sdw(mSegmentationId);
        OmSegmentation & seg = sdw.GetSegmentation();

        mOldThreshold = seg.GetDendThreshold();
        seg.SetDendThreshold(mThreshold);
        OmEvents::SegmentModified();
    }

    void Undo()
    {
        SegmentationDataWrapper sdw(mSegmentationId);
        OmSegmentation & seg = sdw.GetSegmentation();

        seg.SetDendThreshold(mOldThreshold);
        OmEvents::SegmentModified();
    }

    std::string Description() const {
        return "Threshold: " + om::string::num(mThreshold);
    }

    QString classNameForLogFile() const {
        return "OmSegmentationThresholdChangeAction";
    }

private:
    template <typename T> friend class OmActionLoggerThread;

    friend class QDataStream
    &operator<<(QDataStream&, const OmSegmentationThresholdChangeActionImpl&);

    friend class QDataStream
    &operator>>(QDataStream&, OmSegmentationThresholdChangeActionImpl&);
};

#endif
