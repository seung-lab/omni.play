#pragma once
#include "precomp.h"

#include "gui/inspectors/volInspector.h"
#include "utility/segmentationDataWrapper.hpp"
#include "gui/inspectors/absCoordBox.hpp"

namespace om {
namespace segmentationInspector {

class PageMetadata : public QWidget {
  Q_OBJECT;

 private:
  const SegmentationDataWrapper sdw_;

 public:
  PageMetadata(QWidget* parent, const SegmentationDataWrapper& sdw)
      : QWidget(parent), sdw_(sdw) {
    QVBoxLayout* overallContainer = new QVBoxLayout(this);
    overallContainer->addWidget(makeVolBox());
    overallContainer->addWidget(makeAbsCoordBox());
    overallContainer->addStretch(1);
  }

 private:
  QGroupBox* makeVolBox() {
    return new OmVolInspector(sdw_.GetSegmentation(), this);
  }

  QGroupBox* makeAbsCoordBox() {
    return new om::volumeInspector::AbsCoordBox(sdw_.GetSegmentation(), this);
  }
};

}  // namespace segmentationInspector
}  // namespace om
