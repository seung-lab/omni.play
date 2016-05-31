#pragma once

/*
 * Pure Interface for views to provide methods to assist with
 * input
 */
class SegmentDataWrapper;
namespace om {
  namespace coords {
    class Global;
  }
}
class ViewInputConversion {
 public:
  virtual ~ViewInputConversion() = default;

 public:
  /*
   * Input: x, y coordinates of the screen
   * Returns: optional containing the clicked segment wrapped in
   *   a segmentDataWrapper if found
   */
  virtual boost::optional<SegmentDataWrapper>
    FindSegment(int x, int y) = 0;

  /*
   * Input: x, y coordinates of the screen
   * Returns: optional containing the coordinates in global space
   *   if found
   */
  virtual boost::optional<om::coords::Global>
    FindGlobalCoords(int x, int y) = 0;
};


