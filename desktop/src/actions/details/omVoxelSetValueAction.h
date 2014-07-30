#pragma once
#include "precomp.h"

#include "actions/details/omUndoCommand.hpp"

class OmVoxelSetValueActionImpl;

class OmVoxelSetValueAction : public OmUndoCommand {
 public:
  OmVoxelSetValueAction(std::shared_ptr<OmVoxelSetValueActionImpl> impl)
      : impl_(impl) {}

  OmVoxelSetValueAction(const om::common::ID segmentationId,
                        const std::set<om::coords::Data>& rVoxels,
                        const om::common::SegID value);

 private:
  void Action();
  void UndoAction();
  std::string Description();
  void save(const std::string&);

  std::shared_ptr<OmVoxelSetValueActionImpl> impl_;
};
