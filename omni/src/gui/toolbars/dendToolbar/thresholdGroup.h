#ifndef THRESHOLD_GROUP_H
#define THRESHOLD_GROUP_H

#include <QtGui>

class DendToolBar;
class ThresholdButtonIncrease;
class ThresholdButtonDecrease;

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
	ThresholdButtonDecrease *const thresholdButtonDecrease;
	ThresholdButtonIncrease *const thresholdButtonIncrease;
	QLineEdit* mThreshold;

	void addToThreshold(const float num);
	void setThresholdValue();
	void haveSegmentationChangeThreshold( const float threshold );
};

#endif
