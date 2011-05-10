#pragma once

#include "actions/io/omActionLogger.hpp"
#include "common/omCommon.h"
#include "common/omString.hpp"
#include "events/details/omSegmentEvent.h"
#include "events/omEvents.h"
#include "gui/sidebars/right/dendToolbar.h"
#include "system/omAppState.hpp"
#include "utility/dataWrappers.h"

class OmSegmentationThresholdChangeActionImpl {
private:
    SegmentationDataWrapper sdw_;
    float mThreshold;
    float mOldThreshold;

public:
    OmSegmentationThresholdChangeActionImpl()
    {}

    OmSegmentationThresholdChangeActionImpl(const SegmentationDataWrapper sdw,
                                            const double threshold)
        : sdw_(sdw)
        , mThreshold(threshold)
    {}

    void Execute()
    {
        OmSegmentation & seg = sdw_.GetSegmentation();

        mOldThreshold = seg.GetDendThreshold();
        seg.SetDendThreshold(mThreshold);

        //TODO: don't access view-level directly
        if(OmAppState::GetDendToolBar()){
            OmAppState::GetDendToolBar()->RefreshThreshold();
        }

        OmEvents::SegmentModified();
    }

    void Undo()
    {
        OmSegmentation & seg = sdw_.GetSegmentation();

        seg.SetDendThreshold(mOldThreshold);

        //TODO: don't access view-level directly
        if(OmAppState::GetDendToolBar()){
            OmAppState::GetDendToolBar()->RefreshThreshold();
        }

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

    friend class QDataStream &operator<<(QDataStream&, const OmSegmentationThresholdChangeActionImpl&);
    friend class QDataStream &operator>>(QDataStream&, OmSegmentationThresholdChangeActionImpl&);
    friend class QTextStream &operator<<(QTextStream& out, const OmSegmentationThresholdChangeActionImpl& a);
};

