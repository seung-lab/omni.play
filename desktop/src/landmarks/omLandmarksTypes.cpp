#include "landmarks/omLandmarksTypes.h"

std::string om::landmarks::outputPt(const sdwAndPt& d) {
  const SegmentDataWrapper& sdw = d.sdw;
  const om::globalCoord& coord = d.coord;

  std::stringstream s;
  s << "segmentation: " << sdw.GetSegmentationID() << ", "
    << "segment: " << sdw.GetSegmentID() << " (root: " << sdw.FindRootID()
    << ") "
    << ", "
    << "pt: (" << coord.x << ", " << coord.y << ", " << coord.z << ")\n";

  return s.str();
}
