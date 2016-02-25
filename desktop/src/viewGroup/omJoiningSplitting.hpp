#pragma once
#include "precomp.h"

#include "events/listeners.h"
#include "gui/tools.hpp"
#include "utility/dataWrappers.h"
#include "coordinates/coordinates.h"

class OmJoiningSplitting : public om::event::ToolModeEventListener {
 private:
  om::tool::mode currentTool;
  bool shouldVolumeBeShownBroken_;

  boost::optional<om::coords::Global> firstCoordinate_;
  SegmentDataWrapper firstSegment_;


 public:
  OmJoiningSplitting();

  void SetShouldVolumeBeShownBroken(bool shouldVolumeBeShownBroken);

  bool ShouldVolumeBeShownBroken() const;

  const SegmentDataWrapper& FirstSegment() const;

  const boost::optional<om::coords::Global>& FirstCoord() const;

  // listener for new tool
  void ToolModeChangeEvent();

  // filter events to only listen for SPLIT and JOIN. should do nothing otherwise 
  void ActivateTool(om::tool::mode tool);
  // when we listen to the previous tool that was activated, we will reset 
  // the segment and coordinate parameters
  void DeactivateTool();

  void Reset();

  // Activate this tool and notify listeners (buttons)
  void SetFirstPoint(om::tool::mode tool, const SegmentDataWrapper& sdw,
                          const om::coords::Global& coord);
};
