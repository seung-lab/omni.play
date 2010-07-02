#ifndef OM_THRESHOLD_GROUP_H
#define OM_THRESHOLD_GROUP_H

#include <QtGui>

class DendToolBar;

class OmThresholdGroup : public QDoubleSpinBox {
 Q_OBJECT 
 public:
	OmThresholdGroup(DendToolBar *, const QString &);

	void updateGui();

 private slots:
	void thresholdChanged(const double valueFromGUI);

 protected:
	DendToolBar *const mDendToolbar;

	double getGUIvalue();
	void setGUIvalue(const double newThreshold);

	virtual void setInitialGUIThresholdValue() = 0;
	virtual void actUponThresholdChange( const float threshold ) = 0;
};

#endif
