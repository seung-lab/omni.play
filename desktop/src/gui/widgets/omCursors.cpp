#include "gui/widgets/omCursors.h"
#include "common/common.h"
#include "system/omStateManager.h"
#include "gui/tools.hpp"

void OmCursors::setToolCursor(QWidget* w){
    w->setCursor(figureOutCursor());
}

QCursor OmCursors::figureOutCursor()
{
    switch(OmStateManager::GetToolMode()){
    case om::tool::SELECT:
        return Qt::BlankCursor;
    case om::tool::PAN:
    default:
        return Qt::ArrowCursor;
    }
}
