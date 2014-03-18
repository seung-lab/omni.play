#pragma once
#include "precomp.h"

#include "utility/segmentDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "segment/omSegmentIterator.h"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupView2dState.hpp"

class OmSegmentCenter {
 private:
  boost::
      optional<om::coords::DataBbox> static computeSelectedSegmentBoundingBox(
          const SegmentationDataWrapper& sdw) {
    om::coords::DataBbox box(sdw.GetSegmentation().Coords(), 0);

    OmSegmentIterator iter(sdw.Segments());
    iter.iterOverSelectedIDs();

    const int max = 5000;

    OmSegment* seg = iter.getNextSegment();
    for (int i = 0; i < max && nullptr != seg; ++i) {
      const om::coords::DataBbox segBox = seg->BoundingBox();
      if (segBox.isEmpty()) {
        continue;
      }

      box.merge(segBox);

      seg = iter.getNextSegment();
    }

    if (box.isEmpty()) {
      return boost::optional<om::coords::DataBbox>();
    }

    return boost::optional<om::coords::DataBbox>(box);
  }

  boost::optional<om::coords::Data> static findCenterOfSelectedSegments(
      const SegmentationDataWrapper& sdw) {
    boost::optional<om::coords::DataBbox> box =
        computeSelectedSegmentBoundingBox(sdw);

    if (!box) {
      return boost::optional<om::coords::Data>();
    }

    const om::coords::Data ret = (box->getMin() + box->getMax()) / 2;
    return boost::optional<om::coords::Data>(ret);
  }

  boost::optional<om::coords::Data> static findCenterOfSelectedSegments(
      const SegmentDataWrapper& sdw) {
    if (!sdw.IsSegmentValid()) {
      return boost::optional<om::coords::Data>();
    }

    OmSegment* seg = sdw.GetSegment();
    const om::coords::DataBbox& box = seg->BoundingBox();
    if (box.isEmpty()) {
      return boost::optional<om::coords::Data>();
    }

    const om::coords::Data ret = (box.getMin() + box.getMax()) / 2;
    return boost::optional<om::coords::Data>(ret);
  }

 public:
  static void CenterSegment(OmViewGroupState& vgs) { CenterSegment(&vgs); }

  static void CenterSegment(OmViewGroupState* vgs) {
    const SegmentationDataWrapper sdw = vgs->Segmentation();

    const boost::optional<om::coords::Data> voxelDC =
        findCenterOfSelectedSegments(sdw);

    if (!voxelDC) {
      return;
    }

    vgs->View2dState()->SetScaledSliceDepth(voxelDC->ToGlobal());

    om::event::ViewCenterChanged();
    om::event::View3dRecenter();
  }

  static boost::optional<float> ComputeCameraDistanceForSelectedSegments() {

    om::coords::GlobalBbox box;

    FOR_EACH(iter, SegmentationDataWrapper::ValidIDs()) {
      SegmentationDataWrapper sdw(*iter);

      const boost::optional<om::coords::DataBbox> b =
          computeSelectedSegmentBoundingBox(sdw);

      if (b) {
        box.merge(b->ToGlobalBbox());
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
    log_infos << "rebuilding segment bounding box data...";

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
