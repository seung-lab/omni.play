#ifndef DISPLAY_TOOLS_H
#define DISPLAY_TOOLS_H

#include <QtGui>
class DendToolBar;
class OmViewGroupState;
class SegmentationDataWrapper;

class DisplayTools : public QGroupBox {
 Q_OBJECT
 public:
	DisplayTools(DendToolBar *);

	OmViewGroupState * getViewGroupState();
	SegmentationDataWrapper getSegmentationDataWrapper();
	void updateGui();

 private:
	DendToolBar *const mParent;

	QWidget* thresholdBox();
};

#endif
