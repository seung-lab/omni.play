#ifndef THRESHOLD_BUTTON_DECREASE_H
#define THRESHOLD_BUTTON_DECREASE_H

#include "gui/widgets/omButton.h"
#include "gui/toolbars/dendToolbar/thresholdGroup.h"

class ThresholdButtonDecrease : public OmButton<ThresholdGroup>  {
 Q_OBJECT
 public:
	ThresholdButtonDecrease(ThresholdGroup * tg,
				const QString & title, 
				const QString & statusTip );
 private:
	void doAction();
};

#endif
