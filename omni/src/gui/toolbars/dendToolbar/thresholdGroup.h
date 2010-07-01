#ifndef THRESHOLD_GROUP_H
#define THRESHOLD_GROUP_H

#include <QtGui>

class DendToolBar;

class ThresholdGroup : public QGroupBox {
 Q_OBJECT 
 public:
	ThresholdGroup(DendToolBar * parent);

 private slots:
	void increaseThreshold();
	void decreaseThreshold();
	void thresholdChanged();

 private:
	DendToolBar * mDendToolbar;
	QPushButton * decreaseThresholdAct;
	QPushButton * increaseThresholdAct;
	QLineEdit* mThreshold;

	void updateGui();
	void addToThreshold(const float num);
	void setThresholdValue();
	void haveSegmentationChangeThreshold( const float threshold );
};

#endif
