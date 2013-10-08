#pragma once

#include "actions/details/omUndoCommand.hpp"

class OmVoxelSetValueActionImpl;

class OmVoxelSetValueAction : public OmUndoCommand {
 public:
  OmVoxelSetValueAction(om::shared_ptr<OmVoxelSetValueActionImpl> impl)
      : impl_(impl) {}

  OmVoxelSetValueAction(const OmID segmentationId,
                        const om::globalCoord& rVoxel, const OmSegID value);

  OmVoxelSetValueAction(const OmID segmentationId,
                        const std::set<om::globalCoord>& rVoxels,
                        const OmSegID value);

 private:
  void Action();
  void UndoAction();
  std::string Description();
  void save(const std::string&);

  om::shared_ptr<OmVoxelSetValueActionImpl> impl_;
};
