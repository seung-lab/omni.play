#pragma once

#include "common/common.h"

#include <QtGui>

class GuiUtils{
public:
	static Qt::CheckState getCheckState(const bool enabled)
	{
		if (enabled) {
			return Qt::Checked;
		} else {
			return Qt::Unchecked;
		}
	}

	static bool getBoolState(const Qt::CheckState state)
	{
		switch(state) {
		case Qt::Unchecked:
			return false;
			break;
		case Qt::PartiallyChecked:
		case Qt::Checked:
			return true;
			break;
		default:
			throw OmArgException("unknown state?");
		}
	}

	static void autoResizeColumnWidths(QTreeWidget* widget,
									   const int max_col_to_display)
	{
		for (int i = 0; i <= max_col_to_display; i++) {
			widget->resizeColumnToContents(i);
		}
	}
};

