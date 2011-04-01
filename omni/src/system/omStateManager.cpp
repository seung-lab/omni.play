#include "system/omStateManager.h"
#include "system/omStateManagerImpl.hpp"

OmStateManager::OmStateManager()
{}

OmStateManager::~OmStateManager()
{}

om::tool::mode OmStateManager::GetToolMode(){
    return impl().GetToolMode();
}

void OmStateManager::SetToolModeAndSendEvent(const om::tool::mode tool){
    impl().SetToolModeAndSendEvent(tool);
}

void OmStateManager::SetOldToolModeAndSendEvent(){
    impl().SetOldToolModeAndSendEvent();
}

OmUndoStack& OmStateManager::UndoStack(){
    return impl().UndoStack();
}

const QGLWidget* OmStateManager::GetPrimaryView3dWidget(){
    return impl().GetPrimaryView3dWidget();
}

OmBrushSize* OmStateManager::BrushSize() {
    return impl().BrushSize();
}
