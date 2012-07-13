#pragma once

#include "actions/io/omActionLogger.hpp"
#include "common/omCommon.h"
#include "common/omString.hpp"
#include "events/details/omSegmentEvent.h"
#include "events/omEvents.h"
#include "gui/sidebars/right/rightImpl.h"
#include "system/omAppState.hpp"
#include "utility/dataWrappers.h"

class OmAutomaticSpreadingThresholdChangeActionImpl {
private:
    SegmentationDataWrapper sdw_;
    float threshold_;
    float oldThreshold_;

public:
    OmAutomaticSpreadingThresholdChangeActionImpl()
    {}

    OmAutomaticSpreadingThresholdChangeActionImpl(const SegmentationDataWrapper sdw,
                                            const double threshold)
        : sdw_(sdw)
        , threshold_(threshold)
    {}

    void Execute()
    {
        OmSegmentation& seg = sdw_.GetSegmentation();

        oldThreshold_ = seg.GetASThreshold();

        seg.SetASThreshold(threshold_);
    }

    void Undo()
    {
        OmSegmentation& seg = sdw_.GetSegmentation();

        seg.SetASThreshold(oldThreshold_);
    }

    std::string Description() const {
        return "Threshold: " + om::string::num(threshold_);
    }

    QString classNameForLogFile() const {
        return "OmAutomaticSpreadingThresholdChangeAction";
    }

private:
    template <typename T> friend class OmActionLoggerThread;

    friend class QDataStream &operator<<(QDataStream&, const OmAutomaticSpreadingThresholdChangeActionImpl&);
    friend class QDataStream &operator>>(QDataStream&, OmAutomaticSpreadingThresholdChangeActionImpl&);
    friend class QTextStream &operator<<(QTextStream& out, const OmAutomaticSpreadingThresholdChangeActionImpl& a);
};

