#ifndef OM_VIEW2D_KEY_PRESS_EVENT_LISTENER_H
#define OM_VIEW2D_KEY_PRESS_EVENT_LISTENER_H

#include "segment/omSegmentSelected.hpp"
#include "system/events/omKeyPressEventListener.h"


class OmView2dEvents;
class OmScreenPainter;
class SegmentDataWrapper;
class OmScreenShotSaver;
class OmMouseEvents;
class OmKeyEvents;
class OmView2dZoom;

class OmView2dKeyPressEventListener : public OmKeyPressEventListener {

public:
	OmView2dKeyPressEventListener(){}
protected:
};
#endif
