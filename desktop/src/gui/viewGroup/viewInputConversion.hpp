#pragma once

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
    GetSelectedSegment(int x, int y) = 0;

  /*
   * Input: x, y coordinates of the screen
   * Returns: optional containing the coordinates in global space
   *   if found
   */
  virtual boost::optional<om::coords::Global>
    GetGlobalCoords(int x, int y) = 0;
};


