#ifndef GRAPH_TOOLS_H
#define GRAPH_TOOLS_H

#include <QtGui>

class DendToolBar;
class AutoBreakCheckbox;
class SplitButton;
class OmViewGroupState;
class SegmentationDataWrapper;

class GraphTools : public QGroupBox {
 Q_OBJECT
 public:
	GraphTools(DendToolBar *);
	void SetSplittingOff();

	OmViewGroupState * getViewGroupState();
	SegmentationDataWrapper getSegmentationDataWrapper();
	void updateGui();

 private:
	DendToolBar *const mParent;
	AutoBreakCheckbox * autoBreakCheckbox;
	SplitButton * splitButton;
};

#endif
