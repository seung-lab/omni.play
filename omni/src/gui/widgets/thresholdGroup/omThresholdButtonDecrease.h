#ifndef OM_THRESHOLD_BUTTON_DECREASE_H
#define OM_THRESHOLD_BUTTON_DECREASE_H

#include "gui/widgets/omButton.h"

template < class T >
class OmThresholdButtonDecrease : public OmButton<T>  {
 public:
	OmThresholdButtonDecrease(T *,
				  const QString & title, 
				  const QString & statusTip );
 protected:
	virtual void doAction();
};

#endif
