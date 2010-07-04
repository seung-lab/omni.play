#include "gui/widgets/omCursors.h"
#include "common/omCommon.h"
#include "system/omStateManager.h"

void OmCursors::setToolCursor(QWidget* w)
{
	QCursor cursor;

	switch(OmStateManager::GetToolMode()){
	case SELECT_MODE:
		cursor = Qt::PointingHandCursor;
		break;
	case PAN_MODE:
		cursor = Qt::OpenHandCursor;
		break;
	case CROSSHAIR_MODE:
		cursor = Qt::CrossCursor;
		break;
	case ZOOM_MODE:
		cursor = Qt::ArrowCursor;
		break;
	default:
		cursor = Qt::ArrowCursor;
	}

	w->setCursor(cursor);
}
