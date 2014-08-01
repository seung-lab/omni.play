#pragma once
#include "precomp.h"

#include "actions/omActions.h"
#include "datalayer/file.h"
#include "gui/widgets/omButton.hpp"
#include "landmarks/omLandmarksTypes.h"
#include "landmarks/omLandmarksDialog.h"
#include "landmarks/omLandmarksWidget.h"
#include "utility/segmentationDataWrapper.hpp"
#include "volume/omSegmentation.h"
#include "users/omUsers.h"

namespace om {
namespace landmarks {

class ClearButton : public OmButton<om::landmarks::widget> {
 private:
  dialog* const dialog_;

 public:
  ClearButton(widget* widget, dialog* dialog)
      : OmButton<om::landmarks::widget>(widget, "Clear", "Clear", false),
        dialog_(dialog) {}

 private:
  void doAction() { dialog_->ClearPtsAndHide(); }
};

class LongRangeConnectionButton : public OmButton<om::landmarks::widget> {
 private:
  dialog* const dialog_;
  const std::vector<sdwAndPt> pts_;

 public:
  LongRangeConnectionButton(widget* widget, dialog* dialog,
                            const std::vector<sdwAndPt>& pts)
      : OmButton<om::landmarks::widget>(widget, "Make Long Range Connection",
                                        "Make Long Range Connection", false),
        dialog_(dialog),
        pts_(pts) {}

 private:
  void doAction() {
    const SegmentationDataWrapper sdw =
        pts_[0].sdw.MakeSegmentationDataWrapper();
    OmSegmentation* vol = sdw.GetSegmentation();

    const QString fpath = OmProject::Globals()
                              .Users()
                              .UserPaths()
                              .LongRangeConnections(vol->GetID())
                              .c_str();

    QFile file(fpath);

    if (!file.open(QIODevice::Append)) {
      throw om::IoException("could not open file for write");
    }

    QTextStream out(&file);

    out << "\n";
    out << QString::fromStdString(outputPt(pts_[0]));
    out << "\t" << QString::fromStdString(outputPt(pts_[1]));
    out << "\n";

    dialog_->ClearPtsAndHide();
  }
};

class JoinButton : public OmButton<om::landmarks::widget> {
 private:
  dialog* const dialog_;
  const std::vector<sdwAndPt> pts_;

 public:
  JoinButton(widget* widget, dialog* dialog, const std::vector<sdwAndPt>& pts)
      : OmButton<om::landmarks::widget>(widget, "Join", "Join", false),
        dialog_(dialog),
        pts_(pts) {}

 private:
  void doAction() {
    FOR_EACH(id, SegmentationDataWrapper::ValidIDs()) {
      const om::common::ID segmentationID = *id;

      om::common::SegIDSet set;

      FOR_EACH(iter, pts_) {
        const SegmentDataWrapper& sdw = iter->sdw;

        if (sdw.GetSegmentationID() != segmentationID) {
          continue;
        }

        set.insert(sdw.FindRootID());
      }

      SegmentationDataWrapper sdw(segmentationID);

      OmActions::JoinSegments(sdw, set);
    }

    dialog_->ClearPtsAndHide();
  }
};

}  // namespace landmarks
}  // namespace om
