#ifndef GRAPH_TOOLS_H
#define GRAPH_TOOLS_H

#include <QtGui>
#include "gui/widgets/omWidget.hpp"

class DendToolBar;
class AutoBreakCheckbox;
class SplitButton;
class OmViewGroupState;
class SegmentationDataWrapper;

class GraphTools : public OmWidget {
 Q_OBJECT
 public:
	GraphTools(DendToolBar *);
	void SetSplittingOff();

	OmViewGroupState * getViewGroupState();
	SegmentationDataWrapper GetSegmentationDataWrapper();
	void updateGui();

	QString getName(){ return "Graph Tools"; }

 private:
	DendToolBar *const mParent;
	AutoBreakCheckbox * autoBreakCheckbox;
	SplitButton * splitButton;

	QWidget* thresholdBox();
	QWidget* breakThresholdBox();

};

#endif
