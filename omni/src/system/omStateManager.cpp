#include "system/omStateManager.h"
#include "system/omStateManagerImpl.hpp"

OmStateManager::OmStateManager()
{}

OmStateManager::~OmStateManager()
{}

OmToolMode OmStateManager::GetToolMode(){
	return impl().GetToolMode();
}

void OmStateManager::SetToolModeAndSendEvent(const OmToolMode tool){
	impl().SetToolModeAndSendEvent(tool);
}

void OmStateManager::SetOldToolModeAndSendEvent(){
	impl().SetOldToolModeAndSendEvent();
}

QUndoStack* OmStateManager::GetUndoStack(){
	return impl().GetUndoStack();
}

void OmStateManager::PushUndoCommand(QUndoCommand* cmd){
	impl().PushUndoCommand(cmd);
}

void OmStateManager::ClearUndoStack(){
	impl().ClearUndoStack();
}

void OmStateManager::UndoUndoCommand(){
	impl().UndoUndoCommand();
}

const QGLWidget* OmStateManager::GetPrimaryView3dWidget(){
	return impl().GetPrimaryView3dWidget();
}
