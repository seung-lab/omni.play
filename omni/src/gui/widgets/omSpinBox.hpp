#ifndef OM_THRESHOLD_GROUP_H
#define OM_THRESHOLD_GROUP_H

#include "common/omDebug.h"
#include "gui/widgets/omCursors.h"
#include "system/omEvents.h"

#include <QtGui>

namespace om {
	enum ShouldUpdateAsType{
		UPDATE_AS_TYPE,
		DONT_UPDATE_AS_TYPE
	};
};

class OmSpinBox : public QDoubleSpinBox {
Q_OBJECT
public:
	OmSpinBox(QWidget * d,
			  const om::ShouldUpdateAsType updateAsType)
		: QDoubleSpinBox(d)
	{
		if(om::UPDATE_AS_TYPE == updateAsType){
			connect(this, SIGNAL(valueChanged(double)),
					this, SLOT(thresholdChanged()),
					Qt::DirectConnection );
		} else {
			connect(this, SIGNAL(editingFinished()),
					this, SLOT(thresholdChanged()),
					Qt::DirectConnection );
		}
	}

	void updateGui(){
		OmEvents::Redraw();
	}

 private slots:
	void thresholdChanged()
	{
		OmBusyCursorWrapper busyCursorWrapper();
		actUponThresholdChange(getGUIvalue());
		updateGui();
	}

 protected:
	double getGUIvalue(){
		return value();
	}

	void setGUIvalue(const double newThreshold){
		setValue(newThreshold);
	}

	virtual void setInitialGUIThresholdValue() = 0;
	virtual void actUponThresholdChange( const float threshold ) = 0;
};

#endif
