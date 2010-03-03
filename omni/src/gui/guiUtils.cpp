#include "guiUtils.h"

Qt::CheckState GuiUtils::getCheckState(const bool enabled)
{
	if (enabled) {
		return Qt::Checked;
	} else {
		return Qt::Unchecked;
	}
}

bool GuiUtils::getBoolState(const Qt::CheckState state)
{
	switch(state ) {
	case Qt::Unchecked:
		return false;
	case Qt::PartiallyChecked:
	case Qt::Checked:
		return true;
	}
}

