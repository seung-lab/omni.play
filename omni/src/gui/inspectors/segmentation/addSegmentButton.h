#ifndef ADD_SEGMENT_BUTTON_H
#define ADD_SEGMENT_BUTTON_H

#include "gui/widgets/omButton.h"
#include "gui/inspectors/segInspector.h"

class AddSegmentButton : public OmButton<SegInspector> {
 public:
	AddSegmentButton(SegInspector *);

 private:
	void doAction();
};

#endif
