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
#include "events/listeners.h"

namespace om {
namespace gui {

class SegListToggleButton : public OmButton<QWidget>,
                            public om::event::SegmentEventListener {
 public:
  SegListToggleButton(QWidget* d, std::string name, common::SegIDSet segIDs)
      : parent_(d),
        OmButton<QWidget>(d, QString::fromStdString(name), "", false),
        segIDs_(segIDs) {
      if (name.find("Seed") != std::string::npos){
          this->setCheckable(true);
          this->setChecked(true);

          setObjectName("seed");
      }
  }

  void SegmentModificationEvent(om::event::SegmentEvent*) {}
  void SegmentGUIlistEvent(om::event::SegmentEvent*) {}
  void SegmentSelectedEvent(om::event::SegmentEvent* event) {
      SegmentationDataWrapper sdw(1); //TODO correct this segmentation id hack
      segment::Selection &currentSelection = sdw.Segments()->Selection();

      if(objectName() == "seed"){
          bool seedOn = true;
          for( auto &seg : segIDs_ ){
              if(!currentSelection.IsSegmentSelected(seg)){
                  seedOn = false;
                  break;
              }
          }
          this->setChecked(seedOn);
      }
  }
  void SegmentBrushEvent(om::event::SegmentEvent*) {}
  common::SegIDSet segIDs_;

 private:
  QWidget* parent_;

  void doAction() override {
    SegmentationDataWrapper sdw(1); //TODO correct this segmentation id hack
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

    if(objectName() != "seed"){
        om::event::SegmentSelected();
    }

  }

  void doRightClick() override { OmTellInfo tell(om::string::join(segIDs_)); }


};


}  // namespace gui
}  // namespace om
