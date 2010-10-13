#include "gui/guiUtils.h"
#include "common/omCommon.h"

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
	switch(state) {
	case Qt::Unchecked:
		return false;
		break;
	case Qt::PartiallyChecked:
	case Qt::Checked:
		return true;
		break;
	}

	throw OmArgException("unknown state?");
}

void GuiUtils::autoResizeColumnWidths(QTreeWidget * widget, const int max_col_to_display)
{
	for (int i = 0; i <= max_col_to_display; i++) {
		widget->resizeColumnToContents(i);
	}
}
