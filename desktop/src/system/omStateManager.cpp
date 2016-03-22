#include "system/omStateManager.h"
#include "system/omStateManagerImpl.hpp"

OmStateManager::OmStateManager() {}

OmStateManager::~OmStateManager() {}

void OmStateManager::ResetTool() { return impl().ResetTool(); }
om::tool::mode OmStateManager::GetToolMode() { return impl().GetToolMode(); }

void OmStateManager::SetToolModeAndSendEvent(const om::tool::mode tool) {
  impl().SetToolModeAndSendEvent(tool);
}

void OmStateManager::SetOldToolModeAndSendEvent() {
  impl().SetOldToolModeAndSendEvent();
}

bool OmStateManager::IsEnabled(const om::tool::mode tool) {
  return impl().IsEnabled(tool);
}

void OmStateManager::EnableTools(const std::set<om::tool::mode> tools, bool isEnabled) {
  impl().EnableTools(tools, isEnabled);
}

void OmStateManager::EnableTool(const om::tool::mode tool, bool isEnabled) {
  impl().EnableTool(tool, isEnabled);
}

OmUndoStack& OmStateManager::UndoStack() { return impl().UndoStack(); }

OmBrushSize* OmStateManager::BrushSize() { return impl().BrushSize(); }
