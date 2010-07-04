#ifndef OM_THRESHOLD_GROUP_H
#define OM_THRESHOLD_GROUP_H

#include <QtGui>

class OmThresholdGroup : public QDoubleSpinBox {
 Q_OBJECT 
 public:
	OmThresholdGroup(QWidget *, const bool updateAsType);

	void updateGui();

 private slots:
	void thresholdChanged();

 protected:
	double getGUIvalue();
	void setGUIvalue(const double newThreshold);

	virtual void setInitialGUIThresholdValue() = 0;
	virtual void actUponThresholdChange( const float threshold ) = 0;
};

#endif
