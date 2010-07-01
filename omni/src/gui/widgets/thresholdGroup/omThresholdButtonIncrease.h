#ifndef OM_THRESHOLD_BUTTON_INCREASE_H
#define OM_THRESHOLD_BUTTON_INCREASE_H

#include "gui/widgets/omButton.h"

template < class T >
class OmThresholdButtonIncrease : public OmButton<T>  {
 public:
	OmThresholdButtonIncrease(T *,
				  const QString & title, 
				  const QString & statusTip );
 protected:
	virtual void doAction();
};

#endif
