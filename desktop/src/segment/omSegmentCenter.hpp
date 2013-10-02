#pragma once

#include "utility/segmentDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "segment/omSegmentIterator.h"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupView2dState.hpp"

class OmSegmentCenter {
 private:

  boost::optional<om::dataBbox> static computeSelectedSegmentBoundingBox(
      const SegmentationDataWrapper& sdw) {
    om::dataBbox box(sdw.GetSegmentationPtr(), 0);

    OmSegmentIterator iter(sdw.Segments());
    iter.iterOverSelectedIDs();

    const int max = 5000;

    OmSegment* seg = iter.getNextSegment();
    for (int i = 0; i < max && NULL != seg; ++i) {
      const om::dataBbox segBox = seg->BoundingBox();
      if (segBox.isEmpty()) {
        continue;
      }

      box.merge(segBox);

      seg = iter.getNextSegment();
    }

    if (box.isEmpty()) {
      return boost::optional<om::dataBbox>();
    }

    return boost::optional<om::dataBbox>(box);
  }

  boost::optional<om::dataCoord> static findCenterOfSelectedSegments(
      const SegmentationDataWrapper& sdw) {
    boost::optional<om::dataBbox> box = computeSelectedSegmentBoundingBox(sdw);

    if (!box) {
      return boost::optional<om::dataCoord>();
    }

    const om::dataCoord ret = (box->getMin() + box->getMax()) / 2;
    return boost::optional<om::dataCoord>(ret);
  }

  boost::optional<om::dataCoord> static findCenterOfSelectedSegments(
      const SegmentDataWrapper& sdw) {
    if (!sdw.IsSegmentValid()) {
      return boost::optional<om::dataCoord>();
    }

    OmSegment* seg = sdw.GetSegment();
    const om::dataBbox& box = seg->BoundingBox();
    if (box.isEmpty()) {
      return boost::optional<om::dataCoord>();
    }

    const om::dataCoord ret = (box.getMin() + box.getMax()) / 2;
    return boost::optional<om::dataCoord>(ret);
  }

 public:
  static void CenterSegment(OmViewGroupState& vgs) {
      CenterSegment(&vgs);
  }

  static void CenterSegment(OmViewGroupState* vgs) {
    const SegmentationDataWrapper sdw = vgs->Segmentation();

    const boost::optional<om::dataCoord> voxelDC =
        findCenterOfSelectedSegments(sdw);

    if (!voxelDC) {
      return;
    }

    vgs->View2dState()->SetScaledSliceDepth(voxelDC->toGlobalCoord());

    om::event::ViewCenterChanged();
    om::event::View3dRecenter();
  }

  static boost::optional<float> ComputeCameraDistanceForSelectedSegments() {
    om::globalBbox box;

    FOR_EACH(iter, SegmentationDataWrapper::ValidIDs()) {
      SegmentationDataWrapper sdw(*iter);

      const boost::optional<om::dataBbox> b =
          computeSelectedSegmentBoundingBox(sdw);

      if (b) {
        box.merge(b->toGlobalBbox());
      }
    }

    if (box.isEmpty()) {
      return boost::optional<float>();
    }

    const float x = box.getMax().x - box.getMin().x;
    const float y = box.getMax().y - box.getMin().y;
    const float z = box.getMax().z - box.getMin().z;

    return boost::optional<float>(sqrt(x * x + y * y + z * z));
  }

  static void RebuildCenterOfSegmentData(const SegmentationDataWrapper& sdw) {
    printf("rebuilding segment bounding box data...\n");

    OmSegments* segments = sdw.Segments();

    for (om::common::SegID i = 1; i <= segments->getMaxValue(); ++i) {
      OmSegment* seg = segments->GetSegment(i);
      if (!seg) {
        continue;
      }

      seg->ClearBoundingBox();
    }

    sdw.GetSegmentation().UpdateVoxelBoundingData();
    sdw.GetSegmentation().Flush();
  }
};
