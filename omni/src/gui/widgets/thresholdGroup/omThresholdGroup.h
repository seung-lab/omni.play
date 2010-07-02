#ifndef OM_THRESHOLD_GROUP_H
#define OM_THRESHOLD_GROUP_H

#include <QtGui>

class DendToolBar;
#include "gui/widgets/thresholdGroup/omThresholdButtonDecrease.h"
#include "gui/widgets/thresholdGroup/omThresholdButtonIncrease.h"

class OmThresholdGroup : public QGroupBox {
 Q_OBJECT 
 public:
	OmThresholdGroup(DendToolBar *);

	virtual void increaseThresholdButtonWasPressed();
	virtual void decreaseThresholdButtonWasPressed();
	virtual void updateGui();

 protected slots:
	virtual void thresholdChanged();

 protected:
	DendToolBar * mDendToolbar;
	OmThresholdButtonDecrease<OmThresholdGroup> *const thresholdButtonDecrease;
	OmThresholdButtonIncrease<OmThresholdGroup> *const thresholdButtonIncrease;
	QLineEdit* mThreshold;

	virtual void addToThreshold(const float num) = 0;
	virtual void setThresholdValue() = 0;
	virtual void haveSegmentationChangeThreshold( const float threshold ) = 0;
};

#endif
