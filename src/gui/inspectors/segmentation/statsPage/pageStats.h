#pragma once

#include "utility/segmentationDataWrapper.hpp"
#include <QtGui>

namespace om {
namespace segmentationInspector {

class PageStats : public QWidget {
private:
    const SegmentationDataWrapper sdw_;

public:
    PageStats(QWidget* parent, const SegmentationDataWrapper& sdw);

    const SegmentationDataWrapper& GetSDW() const {
        return sdw_;
    }

private:        
    QGroupBox* makeStatsBox();        
};

} // namespace segmentationInspector
} // namespace om

