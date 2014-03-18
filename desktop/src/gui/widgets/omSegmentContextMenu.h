#pragma once
#include "precomp.h"

#include "common/common.h"
#include "utility/dataWrappers.h"
#include "view3d.old/omSegmentPickPoint.h"
#include "landmarks/omLandmarksTypes.h"

class OmViewGroupState;

namespace om {
namespace gui {
class progressBarDialog;
}
}

class OmSegmentContextMenu : public QMenu {

  Q_OBJECT;

 public:
  void Refresh(const SegmentDataWrapper& sdw, OmViewGroupState* vgs);
  void Refresh(const SegmentDataWrapper& sdw, OmViewGroupState* vgs,
               const om::coords::Global coord);
  void Refresh(const OmSegmentPickPoint& pickPoint, OmViewGroupState* vgs);
  void Refresh(const om::landmarks::sdwAndPt& pickPoint, OmViewGroupState& vgs);

 protected:
  void addSelectionNames();
  void addSelectionAction();
  void addDendActions();
  void addColorActions();
  void addGroupActions();
  void addPropertiesActions();
  void addDisableAction();

 private
Q_SLOTS:
  void select();
  void unselect();
  void unselectOthers();

  void mergeSegments();
  void splitSegments();
  void cutSegments();
  void randomizeColor();
  void randomizeSegmentColor();
  void setSelectedColor();

  void setValid();
  void setNotValid();
  void showProperties();
  void printChildren();

  void disableSegment();

 private:
  SegmentDataWrapper sdw_;
  om::coords::Global coord_;
  OmViewGroupState* vgs_;

  bool isValid() const;
  bool isUncertain() const;

  void doRefresh();

  static void writeChildrenFile(
      const QString fnp, om::gui::progressBarDialog* dialog,
      std::shared_ptr<std::deque<std::string> > children);
};
