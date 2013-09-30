#pragma once

#include "actions/omActions.h"
#include "gui/inspectors/segmentation/exportPage/pageExport.h"
#include "gui/widgets/omButton.hpp"
#include "segment/omSegmentCenter.hpp"

namespace om {
namespace segmentationInspector {

class RebuildCenterOfSegmentDataButton : public OmButton<PageExport> {
 public:
  RebuildCenterOfSegmentDataButton(PageExport* d)
      : OmButton<PageExport>(d, "Rebuild segment center data", "Rebuild",
                             false) {}

 private:
  void doAction() {
    const SegmentationDataWrapper& sdw = mParent->GetSDW();
    OmSegmentCenter::RebuildCenterOfSegmentData(sdw);
    OmActions::Save();
  }
};

}  // namespace segmentationInspector
}  // namespace om
