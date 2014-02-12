#pragma once

#include "coordinates/coordinates.h"

class SegmentationDataWrapper;
class SegmentDataWrapper;
class OmViewGroupState;

class OmSegmentCenter {
 private:
  boost::optional<
      om::coords::DataBbox> static computeSelectedSegmentBoundingBox(
      const SegmentationDataWrapper&);

  boost::optional<om::coords::Data> static findCenterOfSelectedSegments(
      const SegmentationDataWrapper& sdw);

  boost::optional<om::coords::Data> static findCenterOfSelectedSegments(
      const SegmentDataWrapper& sdw);

 public:
  static void CenterSegment(OmViewGroupState& vgs);
  static boost::optional<float> ComputeCameraDistanceForSelectedSegments();
  static void RebuildCenterOfSegmentData(const SegmentationDataWrapper&);
};
