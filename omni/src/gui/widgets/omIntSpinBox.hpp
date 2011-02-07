#ifndef OM_INT_SPIN_BOX_HPP
#define OM_INT_SPIN_BOX_HPP

#include "common/om.hpp"
#include "common/omDebug.h"
#include "gui/widgets/omCursors.h"
#include "system/omEvents.h"

#include <QtGui>

class OmIntSpinBox : public QSpinBox {
Q_OBJECT
public:
	OmIntSpinBox(QWidget * d,
				 const om::ShouldUpdateAsType updateAsType)
		: QSpinBox(d)
	{
		if(om::UPDATE_AS_TYPE == updateAsType){
			connect(this, SIGNAL(valueChanged(int)),
					this, SLOT(spinboxChanged()),
					Qt::DirectConnection );
		} else {
			connect(this, SIGNAL(editingFinished()),
					this, SLOT(spinboxChanged()),
					Qt::DirectConnection );
		}
	}

 private Q_SLOTS:
	void spinboxChanged()
	{
		OmBusyCursorWrapper busyCursorWrapper();
		actUponSpinboxChange(value());
		OmEvents::Redraw2d();
	}

 protected:
	virtual void setInitialGUIThresholdValue(){}
	virtual void actUponSpinboxChange(const int value) = 0;
};

#endif
