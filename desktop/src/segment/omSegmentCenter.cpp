#include "segment/omSegment.h"
#include "coordinates/data.h"
#include "segment/omSegments.h"
#include "segment/omSegmentCenter.hpp"
#include "segment/omSegmentIterator.h"
#include "utility/segmentDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupView2dState.hpp"
#include "volume/omSegmentation.h"

boost::optional<om::coords::DataBbox>
OmSegmentCenter::computeSelectedSegmentBoundingBox(
    const SegmentationDataWrapper& sdw) {
  auto s = sdw.GetSegmentation();
  if (!s) {
    return boost::optional<om::coords::DataBbox>();
  }
  om::coords::DataBbox box(s->Coords(), 0);

  OmSegmentIterator iter(*sdw.Segments());
  iter.iterOverSelectedIDs();

  const int max = 5000;

  OmSegment* seg = iter.getNextSegment();
  for (auto i = 0; i < max && nullptr != seg; ++i) {
    const auto segBox = seg->BoundingBox();
    if (segBox.isEmpty()) {
      continue;
    }

    box.merge(segBox);

    seg = iter.getNextSegment();
  }

  if (box.isEmpty()) {
    return false;
  }

  return box;
}

boost::optional<om::coords::Data> OmSegmentCenter::findCenterOfSelectedSegments(
    const SegmentationDataWrapper& sdw) {
  auto box = computeSelectedSegmentBoundingBox(sdw);
  if (!box) {
    return false;
  }

  return (box->getMin() + box->getMax()) / 2;
}

boost::optional<om::coords::Data> OmSegmentCenter::findCenterOfSelectedSegments(
    const SegmentDataWrapper& sdw) {
  if (!sdw.IsSegmentValid()) {
    return false;
  }

  OmSegment* seg = sdw.GetSegment();
  const auto box = seg->BoundingBox();
  if (box.isEmpty()) {
    return false;
  }

  return (box.getMin() + box.getMax()) / 2;
}

void OmSegmentCenter::CenterSegment(OmViewGroupState& vgs) {
  const SegmentationDataWrapper sdw = vgs.Segmentation();

  const boost::optional<om::coords::Data> voxelDC =
      findCenterOfSelectedSegments(sdw);

  if (!voxelDC) {
    return;
  }

  vgs.View2dState().SetScaledSliceDepth(voxelDC->ToGlobal());

  om::event::ViewCenterChanged();
  om::event::View3dRecenter();
}

boost::optional<float>
OmSegmentCenter::ComputeCameraDistanceForSelectedSegments() {
  om::coords::GlobalBbox box;

  for (auto id : SegmentationDataWrapper::ValidIDs()) {
    SegmentationDataWrapper sdw(id);

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

  return sqrt(x * x + y * y + z * z);
}

void OmSegmentCenter::RebuildCenterOfSegmentData(
    const SegmentationDataWrapper& sdw) {
  if (!sdw.IsSegmentationValid()) {
    log_debugs(unknown) << "Invalid SegmentationDataWrapper "
                           "(OmSegmentCenter::RebuildCenterOfSegmentData)";
    return;
  }

  log_debugs(unknown) << "rebuilding segment bounding box data...";

  OmSegments* segments = sdw.Segments();

  for (auto i = 1; i <= segments->maxValue(); ++i) {
    OmSegment* seg = segments->GetSegment(i);
    if (!seg) {
      continue;
    }

    seg->ClearBoundingBox();
  }

  sdw.GetSegmentation()->UpdateVoxelBoundingData();
  sdw.GetSegmentation()->Flush();
}
