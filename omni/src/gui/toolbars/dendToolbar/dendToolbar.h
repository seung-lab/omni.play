#ifndef DEND_TOOLBAR_H
#define DEND_TOOLBAR_H

#include "common/omCommon.h"

#include <QtGui>

class MainWindow;
class OmViewGroupState;
class SegmentationDataWrapper;
class GraphTools;

class DendToolBar : public QToolBar {
 Q_OBJECT
 public:
	DendToolBar( MainWindow * mw );
	void updateGuiFromProjectLoadOrOpen(OmViewGroupState *);
	void updateGui();

        void SetSplittingOff();

	OmViewGroupState * getViewGroupState(){ return mViewGroupState; }
	SegmentationDataWrapper getSegmentationDataWrapper();
	static bool GetShowGroupsMode();
	
 private:
	MainWindow *const mMainWindow;
	OmViewGroupState * mViewGroupState;
	GraphTools * graphTools;
};

#endif
