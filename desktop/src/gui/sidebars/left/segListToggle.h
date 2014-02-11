#pragma once

#include <QtGui>
#include "gui/widgets/omButton.hpp"
#include "gui/widgets/omTellInfo.hpp"
#include "system/omConnect.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "events/events.h"

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

    sdw.Segments()->ToggleSegmentSelection(segIDs_);

    om::event::Redraw2d();
    om::event::Redraw3d();
  }

  void doRightClick() override { OmTellInfo tell(om::string::join(segIDs_)); }
};

}  // namespace gui
}  // namespace om
