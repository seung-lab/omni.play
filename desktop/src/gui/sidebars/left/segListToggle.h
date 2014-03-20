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

namespace om {
namespace gui {

class SegListToggleButton : public OmButton<QWidget> {
 public:
  SegListToggleButton(QWidget* d, std::string name, common::SegIDSet segIDs)
      : OmButton<QWidget>(d, QString::fromStdString(name), "", false),
        segIDs_(segIDs) {}

 private:
  common::SegIDSet segIDs_;

  void doAction() override {
    SegmentationDataWrapper sdw(1);
    if (!sdw.IsValidWrapper()) {
      return;
    }
    common::SegIDSet IDs(segIDs_);
    auto maxID = sdw.Segments()->maxValue();
    for (const auto& id : segIDs_) {
      if (id <= 0 || id > maxID) {
        log_errors << "Invalid segment id " << id << " in group \""
                   << text().toStdString() << '"';
        IDs.erase(id);
      }
    }

    sdw.Segments()->Selection().ToggleSegmentSelection(IDs);

    om::event::Redraw2d();
    om::event::Redraw3d();
  }

  void doRightClick() override { OmTellInfo tell(om::string::join(segIDs_)); }
};

}  // namespace gui
}  // namespace om
