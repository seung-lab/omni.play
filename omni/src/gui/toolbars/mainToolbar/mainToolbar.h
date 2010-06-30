#ifndef NAV_AND_EDIT_TOOLBARS_H
#define NAV_AND_EDIT_TOOLBARS_H

#include "common/omCommon.h"

#include <QtGui>

class FilterWidget;
class MainWindow;
class NavAndEditButtons;
class OmViewGroupState;
class OpenViewGroupButton;
class SaveButton;

class MainToolbar : public QWidget {
 Q_OBJECT
		
 public:
	MainToolbar( MainWindow * mw );

	void updateToolbar();

 private:
	MainWindow *const mMainWindow;

	SaveButton *const saveButton;
	NavAndEditButtons *const buttons;
	OpenViewGroupButton *const openViewGroupButton;
	FilterWidget *const filterWidget;

	void addWidgetToToolbar( QWidget * widget );
	void addSeperatorToToolbar();
	void setReadOnlyWidgetsEnabled();
	void setModifyWidgetsEnabled();
};

#endif
