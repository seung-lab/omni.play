#pragma once

#include "actions/details/omUndoCommand.hpp"

class OmVoxelSetValueActionImpl;

class OmVoxelSetValueAction : public OmUndoCommand {
public:
    OmVoxelSetValueAction(om::shared_ptr<OmVoxelSetValueActionImpl> impl)
        : impl_(impl)
    {}

    OmVoxelSetValueAction(const om::common::ID segmentationId,
                          const om::coords::Global& rVoxel,
                          const om::common::SegID value);

    OmVoxelSetValueAction(const om::common::ID segmentationId,
                          const std::set<om::coords::Global>&rVoxels,
                          const om::common::SegID value);

private:
    void Action();
    void UndoAction();
    std::string Description();
    void save(const std::string&);

    om::shared_ptr<OmVoxelSetValueActionImpl> impl_;
};

