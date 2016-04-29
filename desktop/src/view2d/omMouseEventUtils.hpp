#pragma once
#include "precomp.h"

#include "view2d/omView2dZoom.hpp"
#include "view2d/omView2dState.hpp"
#include "view2d/omMouseEventState.hpp"
#include "view2d/omView2dState.hpp"
#include "utility/dataWrappers.h"
#include "segment/actions/omJoinSplitRunner.hpp"

class OmView2d;

namespace om {
namespace mouse {
namespace event {

  void PickToolAddToSelection(const SegmentDataWrapper& sdw,
                                     const om::coords::Global& dataClickPoint,
                                     OmView2d* v2d) {
    const om::common::SegID segID = sdw.GetVoxelValue(dataClickPoint);
    if (segID) {
      OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), v2d,
                            "view2dpick");
      sel.augmentSelectedSet(segID, true);
    }
  }

  boost::optional<SegmentDataWrapper> getSelectedSegmentSegmentation(
      OmSegmentation* segmentation, om::coords::Global dataClickPoint) {
    const om::common::SegID segmentID =
        segmentation->GetVoxelValue(dataClickPoint);

    if (!segmentID) {
      return boost::optional<SegmentDataWrapper>();
    }

    SegmentDataWrapper ret(segmentation, segmentID);
    return boost::optional<SegmentDataWrapper>(ret);
  }

  boost::optional<SegmentDataWrapper> getSelectedSegment(OmView2dState& state,
      om::coords::Global dataClickPoint) {
    OmMipVolume* vol = &state.getVol();

    if (om::common::SEGMENTATION == vol->getVolumeType()) {
      OmSegmentation* seg = reinterpret_cast<OmSegmentation*>(vol);
      return getSelectedSegmentSegmentation(seg, dataClickPoint);
    }

    boost::optional<SegmentDataWrapper> ret;

    OmChannel* chan = reinterpret_cast<OmChannel*>(vol);
    const std::vector<OmFilter2d*> filters = chan->GetFilters();

    FOR_EACH(iter, filters) {
      OmFilter2d* filter = *iter;

      if (om::OVERLAY_SEGMENTATION == filter->FilterType()) {
        ret = getSelectedSegmentSegmentation(filter->GetSegmentation(), dataClickPoint);

        if (ret) {
          break;
        }
      }
    }
    return ret;
  }

  void doJoinSplitSegment(OmView2dState& state, om::coords::Global dataClickPoint, 
      om::tool::mode tool) {
    boost::optional<SegmentDataWrapper> sdw =
     getSelectedSegment(state, dataClickPoint);

    if (!sdw) {
      return;
    }

    om::JoinSplitRunner::SelectSegment(&state.getViewGroupState(), tool, sdw.get_ptr());
  }
} //namespace event
} //namespace mouse
} //namespace om
