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
	bool ret;

	switch(state) {
	case Qt::Unchecked:
		ret = false;
		break;
	case Qt::PartiallyChecked:
	case Qt::Checked:
		ret = true;
		break;
	}

	return ret;
}

void GuiUtils::autoResizeColumnWidths(QTreeWidget * widget, const int max_col_to_display)
{
	for (int i = 0; i <= max_col_to_display; i++) {
		widget->resizeColumnToContents(i);
	}
}
