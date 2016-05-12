#pragma once
#include "precomp.h"

#include "gui/widgets/omButton.hpp"
#include "gui/widgets/omTellInfo.hpp"
#include "system/omConnect.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "events/events.h"
#include "segment/omSegments.h"
#include "segment/selection.hpp"
#include "common/string.hpp"
#include "segment/omSegmentSelector.h"

namespace om {
namespace gui {

class SegListToggleButton : public OmButton {
 public:
  SegListToggleButton(QWidget* d, std::string name, common::SegIDSet segIDs)
      : OmButton(d, QString::fromStdString(name), "", false),
        segIDs_(segIDs) {}

 private:
  common::SegIDSet segIDs_;

  void onLeftClick() override {
    SegmentationDataWrapper sdw(1);
    if (!sdw.IsValidWrapper()) {
      return;
    }
    auto maxID = sdw.Segments()->maxValue();
    OmSegmentSelector selector(sdw, this, text().toStdString());
    for (const auto& id : segIDs_) {
      if (id <= 0 || id > maxID) {
        log_errors << "Invalid segment id " << id << " in group \""
                   << text().toStdString() << '"';
      } else {
        selector.augmentSelectedSet_toggle(id);
      }

    }
  }

  void onRightClick() override { OmTellInfo tell(om::string::join(segIDs_)); }
};

}  // namespace gui
}  // namespace om
