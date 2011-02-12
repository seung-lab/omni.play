#ifndef OM_SEGMENT_LIST_KEY_PRESS_EVENT_LISTENER_H
#define OM_SEGMENT_LIST_KEY_PRESS_EVENT_LISTENER_H

#include "segment/omSegmentSelected.hpp"
#include "system/events/omKeyPressEventListener.h"

class SegmentListKeyPressEventListener : public OmKeyPressEventListener {
public:
	SegmentListKeyPressEventListener(){}

protected:
	virtual void PressDown(QKeyEvent*){
		printf("segment list key down\n");
	}

	virtual void PressUp(QKeyEvent*){
		printf("segment list key up\n");
	}
};

#endif
