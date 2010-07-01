#ifndef THRESHOLD_GROUP_H
#define THRESHOLD_GROUP_H

#include <QtGui>

class DendToolBar;
#include "gui/widgets/thresholdGroup/omThresholdButtonDecrease.h"
#include "gui/widgets/thresholdGroup/omThresholdButtonIncrease.h"

class ThresholdGroup : public QGroupBox {
 Q_OBJECT 
 public:
	ThresholdGroup(DendToolBar * parent);
	void decreaseThresholdByEpsilon();
	void increaseThresholdByEpsilon();
	void updateGui();

 private slots:
	void thresholdChanged();

 private:
	DendToolBar * mDendToolbar;
	OmThresholdButtonDecrease<ThresholdGroup> *const thresholdButtonDecrease;
	OmThresholdButtonIncrease<ThresholdGroup> *const thresholdButtonIncrease;
	QLineEdit* mThreshold;

	void addToThreshold(const float num);
	void setThresholdValue();
	void haveSegmentationChangeThreshold( const float threshold );
};

#endif
