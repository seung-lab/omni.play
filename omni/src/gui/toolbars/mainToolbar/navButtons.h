#ifndef NAV_BUTTONS_H
#define NAV_BUTTONS_H

#include "common/omCommon.h"
#include <QWidget>

class NavAndEditButtonGroup;
class MainWindow;

class NavAndEditButtons : public QWidget {
 public:
	explicit NavAndEditButtons(MainWindow *);
	virtual ~NavAndEditButtons();

	void addTheButtons();
	void setReadOnlyWidgetsEnabled(const bool toBeEnabled);
	void setModifyWidgetsEnabled(const bool toBeEnabled);

 private:
	MainWindow * mMainWindow;
	NavAndEditButtonGroup * mButtonGroup;
};

#endif
