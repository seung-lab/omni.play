#ifndef OM_THRESHOLD_GROUP_H
#define OM_THRESHOLD_GROUP_H

#include <QtGui>

class DendToolBar;
#include "gui/widgets/thresholdGroup/omThresholdButtonDecrease.h"
#include "gui/widgets/thresholdGroup/omThresholdButtonIncrease.h"

class OmThresholdGroup : public QGroupBox {
 Q_OBJECT 
 public:
	OmThresholdGroup(DendToolBar *, const QString &);

	void increaseThresholdButtonWasPressed();
	void decreaseThresholdButtonWasPressed();
	void updateGui();

 protected slots:
	void thresholdChanged();

 protected:
	DendToolBar *const mDendToolbar;

	OmThresholdButtonDecrease<OmThresholdGroup> *const thresholdButtonDecrease;
	OmThresholdButtonIncrease<OmThresholdGroup> *const thresholdButtonIncrease;
	QLineEdit* mThreshold;

	void addToThreshold(const float num);
	virtual void setThresholdValue() = 0;
	virtual void actUponThresholdChange( const float threshold ) = 0;
	virtual float getThresholdEpsilon() = 0;
	virtual float filterUserEnteredThreshold(const float threshold) = 0;

};

#endif
