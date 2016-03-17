#pragma once
#include "precomp.h"

#include "actions/omActions.h"
#include "gui/inspectors/segmentation/exportPage/pageExport.h"
#include "gui/widgets/omButton.hpp"
#include "segment/omSegmentCenter.hpp"
#include "gui/widgets/segmentationDataWrapperButton.hpp"

namespace om {
namespace segmentationInspector {

class RebuildCenterOfSegmentDataButton : public SegmentationDataWrapperButton {
 public:
  RebuildCenterOfSegmentDataButton(QWidget* widget,
      SegmentationDataWrapper& segmentationDataWrapper)
      : SegmentationDataWrapperButton(widget, "Rebuild segment center data",
          "Rebuild", false, segmentationDataWrapper) {}

 private:
  void onLeftClick() override {
    OmSegmentCenter::RebuildCenterOfSegmentData(GetSegmentationDataWrapper());
    OmActions::Save();
  }
};

}  // namespace segmentationInspector
}  // namespace om
