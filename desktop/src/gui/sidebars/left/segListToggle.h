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

/**
 * @brief The SegListToggleButton class
 * This class specify the buttons which are on the left pannel
 * If you open a file no button are constructed
 * If you select a task to trace only the seed button is contructed
 * If you select a task to compare a seed button,
 * two buttons for each user who trace this task ( difference and dust )
 * and All button are built.
 */
class SegListToggleButton : public OmButton<QWidget>,
                            public om::event::SegmentEventListener {
 public:
  SegListToggleButton(QWidget* d, std::string name, common::SegIDSet segIDs)
      : OmButton<QWidget>(d, QString::fromStdString(name), "", false),
        segIDs_(segIDs) {
      if (name.find("Seed") != std::string::npos){
          setCheckable(true);
          setChecked(true);
          setObjectName("seed");
      }
  }

  void SegmentModificationEvent(om::event::SegmentEvent*) {}
  void SegmentGUIlistEvent(om::event::SegmentEvent*) {}

  /**
   * @brief SegmentSelectedEvent
   * @param event
   *
   * Whenever the current selection change a segmentSelectedEvent is called,
   * we check if the segIDs_ are part of the new current selection and based on that
   * we set this button as checked or not.
   */
  void SegmentSelectedEvent(om::event::SegmentEvent* event) {
      if(objectName() == "seed"){
          SegmentationDataWrapper sdw(1); //TODO correct this segmentation id hack
          segment::Selection &currentSelection = sdw.Segments()->Selection();

          bool seedOn = true;
          for( auto &seg : segIDs_ ){
              if(!currentSelection.IsSegmentSelected(seg)){
                  seedOn = false;
                  break;
              }
          }
          setChecked(seedOn);
     }
  }
  void SegmentBrushEvent(om::event::SegmentEvent*) {}
  common::SegIDSet segIDs_;

 private:
  void doAction() override {

      //Resets the button state after manually clicking on it
      if(objectName() == "seed"){
          setChecked(false);
          setDown(false);
        }

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
  }

  void doRightClick() override { OmTellInfo tell(om::string::join(segIDs_)); }


};


}  // namespace gui
}  // namespace om
