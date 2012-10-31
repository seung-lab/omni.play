#include "actions/io/omActionLogger.hpp"
#include "actions/details/omVoxelSetValueAction.h"
#include "actions/details/omVoxelSetValueActionImpl.hpp"

OmVoxelSetValueAction::OmVoxelSetValueAction(const OmID segmentationId,
                                             const om::coords::Global& rVoxel,
                                             const OmSegID value)
    : impl_(om::make_shared<OmVoxelSetValueActionImpl>(segmentationId,
                                                          rVoxel,
                                                          value))
{
    mUndoable = false;
}

OmVoxelSetValueAction::OmVoxelSetValueAction(const OmID segmentationId,
                                             const std::set<om::coords::Global>& rVoxels,
                                             const OmSegID value)
    : impl_(om::make_shared<OmVoxelSetValueActionImpl>(segmentationId,
                                                          rVoxels,
                                                          value))
{
    mUndoable = false;
}

void OmVoxelSetValueAction::Action(){
    impl_->Execute();
}

void OmVoxelSetValueAction::UndoAction(){
    impl_->Undo();
}

std::string OmVoxelSetValueAction::Description(){
    return impl_->Description();
}

void OmVoxelSetValueAction::save(const std::string & comment){
    OmActionLogger::save(impl_, comment);
}

