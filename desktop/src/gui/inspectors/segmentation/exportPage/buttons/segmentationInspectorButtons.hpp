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
  RebuildCenterOfSegmentDataButton(QWidget* d,
      const SegmentationDataWrapper& segmentationDataWrapper)
      : SegmentationDataWrapperButton(d, "Rebuild segment center data", "Rebuild",
          false, segmentationDataWrapper) {}

 private:
  void onLeftClick() override {
    const SegmentationDataWrapper& sdw = GetSegmentationDataWrapper();
    OmSegmentCenter::RebuildCenterOfSegmentData(sdw);
    OmActions::Save();
  }
};

}  // namespace segmentationInspector
}  // namespace om
