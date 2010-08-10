#ifndef DISPLAY_TOOLS_H
#define DISPLAY_TOOLS_H

#include <QtGui>
#include "gui/widgets/omWidget.hpp"

class DendToolBar;
class OmViewGroupState;
class SegmentationDataWrapper;

class DisplayTools : public OmWidget {
 Q_OBJECT
 public:
	DisplayTools(DendToolBar *);

	OmViewGroupState * getViewGroupState();
	SegmentationDataWrapper getSegmentationDataWrapper();
	void updateGui();

	QString getName(){ return "Display Tools"; }

 private slots:
        void changeMapColors();

 private:
	DendToolBar *const mParent;

	QButtonGroup * validGroup;
        QRadioButton * showValid;
        QRadioButton * dontShowValid;

	QWidget* filterShowNonSelectedSegmentsBox();
	QWidget* thresholdBox();
};

#endif
