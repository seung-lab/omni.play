#pragma once
#include "precomp.h"

#include "utility/segmentationDataWrapper.hpp"

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
