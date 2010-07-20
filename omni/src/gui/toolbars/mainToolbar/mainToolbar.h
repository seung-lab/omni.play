#ifndef MAIN_TOOLBAR_H
#define MAIN_TOOLBAR_H

#include "common/omCommon.h"

#include <QtGui>

class FilterWidget;
class MainWindow;
class NavAndEditButtons;
class OmViewGroupState;
class OpenViewGroupButton;
class SaveButton;
class VolumeRefreshButton;

class MainToolbar : public QWidget {
 Q_OBJECT
		
 public:
	MainToolbar( MainWindow * mw );

	void updateToolbar();
	void setTool(const OmToolMode tool);
	
 private:
	MainWindow *const mMainWindow;

	SaveButton *const saveButton;
	NavAndEditButtons *const buttons;
	OpenViewGroupButton *const openViewGroupButton;
	FilterWidget *const filterWidget;
	VolumeRefreshButton *const volumeRefreshButton;

	void addWidgetToToolbar( QWidget * widget );
	void addSeperatorToToolbar();
	void setReadOnlyWidgetsEnabled();
	void setModifyWidgetsEnabled();
};

#endif
