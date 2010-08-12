#ifndef DEND_TOOLBAR_H
#define DEND_TOOLBAR_H

#include "common/omCommon.h"
#include "gui/widgets/omWidget.hpp"

#include <QtGui>

class MainWindow;
class OmViewGroupState;
class SegmentationDataWrapper;
class GraphTools;
class ValidationGroup;
class DisplayTools;

class DendToolBar : public QToolBar {
 Q_OBJECT
 public:
	DendToolBar( MainWindow* mw, OmViewGroupState* vgs );

	void updateToolBarsPos(QPoint oldPos);

	void updateGui();

        void SetSplittingOff();

	OmViewGroupState * getViewGroupState(){ return mViewGroupState; }
	SegmentationDataWrapper getSegmentationDataWrapper();
	static bool GetShowGroupsMode();

 private:
	MainWindow *const mMainWindow;

	OmViewGroupState * mViewGroupState;

	GraphTools* graphTools;
	ValidationGroup* validationGroup;
	DisplayTools* displayTools;

	QDockWidget* graphToolsDock;
	QDockWidget* validationGroupDock;
	QDockWidget* displayToolsDock;

	QWidget* wrapWithGroupBox(OmWidget* widget);
	QDockWidget* makeDockWidget(OmWidget* widget);
	int moveToolDock(QDockWidget* toolDock, QPoint );
	QPoint recalcPos(QPoint oldPos, const int height);
};

#endif
