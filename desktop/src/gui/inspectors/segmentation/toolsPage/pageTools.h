#pragma once

#include "utility/segmentationDataWrapper.hpp"
#include <QtGui>

namespace om {
namespace segmentationInspector {

class PageTools : public QWidget {
 private:
  const SegmentationDataWrapper sdw_;

 public:
  PageTools(QWidget* parent, const SegmentationDataWrapper& sdw);

  const SegmentationDataWrapper& GetSDW() const { return sdw_; }

 private:
  QGroupBox* makeToolsBox();
};

}  // namespace segmentationInspector
}  // namespace om
