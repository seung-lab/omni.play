#ifndef GUI_UTILS_H
#define GUI_UTILS_H

#include <QtGui>

class GuiUtils
{
 public:
	static Qt::CheckState getCheckState(const bool enabled);
	static bool getBoolState(const Qt::CheckState state);
};

#endif
