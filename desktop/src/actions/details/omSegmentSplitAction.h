#pragma once
#include "precomp.h"

#include "actions/details/omUndoCommand.hpp"
#include "segment/types.hpp"

class SegmentDataWrapper;
class SegmentationDataWrapper;
class OmSegmentation;
class OmSegment;
class OmViewGroupState;
class OmSegmentSplitActionImpl;

class OmSegmentSplitAction : public OmUndoCommand {

 public:
  OmSegmentSplitAction(std::shared_ptr<OmSegmentSplitActionImpl> impl)
      : impl_(impl) {}

  OmSegmentSplitAction(const SegmentationDataWrapper& sdw,
                       const om::segment::UserEdge& edge);

 private:
  void Action();
  void UndoAction();
  std::string Description();
  void save(const std::string& comment);

  std::shared_ptr<OmSegmentSplitActionImpl> impl_;
};
