#pragma once

#include "precomp.h"
#include "gui/controls/inputContext.hpp"
#include "gui/controls/viewStateInputContext.hpp"
#include "gui/controls/findSegment.hpp"
#include "gui/tools.hpp"
#include "segment/types.hpp"

class OmViewGroupState;
class SegmentDataWrapper;
class OmSegmentSelector;

class GrowInputContext
: public InputContext,
  public ViewStateInputContext,
  public FindSegment {
 public:
  GrowInputContext(OmViewGroupState* viewGroupState, om::tool::mode tool,
      std::function<boost::optional<SegmentDataWrapper>(int, int)>
        findSegmentFunction);

  /*
   * input handler functions
   */
  virtual bool mousePressEvent(QMouseEvent* mouseEvent) override;
  virtual bool mouseReleaseEvent(QMouseEvent* mouseEvent) override;
  virtual bool mouseDoubleClickEvent(QMouseEvent* mouseEvent) override;
  virtual bool wheelEvent(QWheelEvent *wheelEvent) override;
  virtual bool keyReleaseEvent(QKeyEvent *keyEvent) override;

  // Grow the segment selected at the x and y location
  bool GrowCoordinates(int x, int y);
  // Force grow this selected segment and unblacklist adjacent segments
  bool GrowCoordinatesAndRemoveAdjacentBlacklist(int x, int y);

  // Remove adjacent segments of segment at x,y from selection
  bool TrimAdjacentCoordinates(int x, int y);
  // Remove adjacent segments of segment at x,y from selection and blacklist it
  bool TrimAndBlacklistAdjacentCoordinates(int x, int y);
  // Remove segment at x,y as well as adjacent segments from selection 
  bool TrimAndRemoveCoordinates(int x, int y);
  // Remove segment at x,y as well as adjacent segments from selection and
  // blacklist the selected segment
  bool TrimAndBlacklistCoordinates(int x, int y);

  // Using the curent selection's focused segment, grow the threshold
  bool GrowIncremental(bool isGrowing);

  // Threshold constants for threshold steps
  static const double THRESHOLD_STEP;
  static const double MAX_THRESHOLD;
  static const double MIN_THRESHOLD;

 private:
  om::tool::mode tool_;

  // Grow the selection but also call a function before growing
  bool growHelper(int x, int y, std::function<void(OmSegmentSelector&,
        om::common::SegID)> preGrowFunction);

  // Trim the selection and call a function after trimming
  bool trimHelper(int x, int y, std::function<void(OmSegmentSelector&,
        om::common::SegID)> postTrimFunction);

  // Get the selector and the segment at x and y
  std::tuple<std::shared_ptr<OmSegmentSelector>,
    boost::optional<SegmentDataWrapper>> getSelection(int x, int y);

  // Helper to call the growing operator with viewgroup state properties
  std::tuple<om::common::SegIDList, om::common::SegIDList>
    FindNotSelected(om::common::SegID selectedID, double threshold,
        OmSegmentSelector& selector);

  // Helper to call the growing operator with viewgroup state properties
  om::common::SegIDList FindSelected(om::common::SegIDList seedIDs,
      OmSegmentSelector& selector);

  // grow segments connected to seed up to given threshold
  om::common::SegIDList grow(om::common::SegID seedID, double threshold,
      OmSegmentSelector& selector);

  // trim the segments connected to seed ids
  void trim(OmSegmentSelector& selector, om::common::SegID seedID);
  void trim(OmSegmentSelector& selector, om::common::SegIDList seedIDs);

  // add and remove connected segments from the blacklist
  void blacklistRemoveAdjacent(OmSegmentSelector& selector,
      om::common::SegID seedID);
  void blacklistAddAdjacent(OmSegmentSelector& selector,
      om::common::SegID seedID);
  std::shared_ptr<OmSegmentSelector> getSelector(int x, int y);

  // Updated the threshold in given direction and return the new threshold
  double getUpdatedThreshold(bool isGrowing);

};
