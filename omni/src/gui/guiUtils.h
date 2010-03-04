#ifndef GUI_UTILS_H
#define GUI_UTILS_H

#include <QtGui>

class GuiUtils
{
 public:
	static Qt::CheckState getCheckState(const bool enabled);
	static bool getBoolState(const Qt::CheckState state);
	static void autoResizeColumnWidths(QTreeWidget * widget, const int max_col_to_display);
};

#endif
