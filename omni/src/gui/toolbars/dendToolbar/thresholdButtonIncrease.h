#ifndef THRESHOLD_BUTTON_INCREASE_H
#define THRESHOLD_BUTTON_INCREASE_H

#include "gui/widgets/omButton.h"
#include "gui/toolbars/dendToolbar/thresholdGroup.h"

class ThresholdButtonIncrease : public OmButton<ThresholdGroup>  {
 Q_OBJECT
 public:
	ThresholdButtonIncrease(ThresholdGroup * tg,
				const QString & title, 
				const QString & statusTip );
 private:
	void doAction();
};

#endif
