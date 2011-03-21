#ifndef OM_VOXEL_SET_VALUE_ACTION_H
#define OM_VOXEL_SET_VALUE_ACTION_H

#include "actions/details/omUndoCommand.hpp"

class OmVoxelSetValueActionImpl;

class OmVoxelSetValueAction : public OmUndoCommand {
public:
    OmVoxelSetValueAction(boost::shared_ptr<OmVoxelSetValueActionImpl> impl)
        : impl_(impl)
    {}

    OmVoxelSetValueAction(const OmID segmentationId,
                          const DataCoord& rVoxel,
                          const OmSegID value);

    OmVoxelSetValueAction(const OmID segmentationId,
                          const std::set<DataCoord>&rVoxels,
                          const OmSegID value);

private:
    void Action();
    void UndoAction();
    std::string Description();
    void save(const std::string&);

    boost::shared_ptr<OmVoxelSetValueActionImpl> impl_;
};

#endif
