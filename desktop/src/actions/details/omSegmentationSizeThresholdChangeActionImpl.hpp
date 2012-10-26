#pragma once

#include "actions/io/omActionLogger.hpp"
#include "common/common.h"
#include "common/omString.hpp"
#include "events/details/omSegmentEvent.h"
#include "events/omEvents.h"
#include "gui/sidebars/right/rightImpl.h"
#include "system/omAppState.hpp"
#include "utility/dataWrappers.h"

class OmSegmentationSizeThresholdChangeActionImpl {
private:
    SegmentationDataWrapper sdw_;
    double threshold_;
    double oldThreshold_;

public:
    OmSegmentationSizeThresholdChangeActionImpl()
    {}

    OmSegmentationSizeThresholdChangeActionImpl(const SegmentationDataWrapper sdw,
                                                const double threshold)
        : sdw_(sdw)
        , threshold_(threshold)
    {}

    void Execute()
    {
        OmSegmentation& seg = sdw_.GetSegmentation();

        oldThreshold_ = seg.GetSizeThreshold();

        seg.SetSizeThreshold(threshold_);

        OmEvents::RefreshMSTthreshold();

        OmEvents::SegmentModified();
    }

    void Undo()
    {
        OmSegmentation& seg = sdw_.GetSegmentation();

        seg.SetSizeThreshold(oldThreshold_);

        OmEvents::RefreshMSTthreshold();

        OmEvents::SegmentModified();
    }

    std::string Description() const {
        return "Threshold: " + om::string::num(threshold_);
    }

    QString classNameForLogFile() const {
        return "OmSegmentationThresholdChangeAction";
    }

private:
    template <typename T> friend class OmActionLoggerThread;

    friend class QDataStream &operator<<(QDataStream&, const OmSegmentationSizeThresholdChangeActionImpl&);
    friend class QDataStream &operator>>(QDataStream&, OmSegmentationSizeThresholdChangeActionImpl&);
    friend class QTextStream &operator<<(QTextStream& out, const OmSegmentationSizeThresholdChangeActionImpl& a);
};

