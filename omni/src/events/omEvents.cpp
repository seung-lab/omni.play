#include "events/details/omPreferenceEvent.h"
#include "events/details/omSegmentEvent.h"
#include "events/details/omToolModeEvent.h"
#include "events/details/omView3dEvent.h"
#include "events/details/omViewEvent.h"
#include "events/details/omEvent.h"
#include "events/details/omEventManager.h"
#include "events/omEvents.h"

#define POST OmEventManager::PostEvent

void OmEvents::ToolChange(){
    POST(new OmToolModeEvent(OmToolModeEvent::TOOL_MODE_CHANGE));
}

void OmEvents::Redraw2d(){
    POST(new OmViewEvent(OmViewEvent::REDRAW));
}

void OmEvents::Redraw2dBlocking(){
    POST(new OmViewEvent(OmViewEvent::REDRAW_BLOCKING));
}

void OmEvents::Redraw3d(){
    POST(new OmView3dEvent(OmView3dEvent::REDRAW));
}

void OmEvents::SegmentSelected(){
    POST(new OmSegmentEvent(OmSegmentEvent::SEGMENT_SELECTED));
}

void OmEvents::SegmentGUIlist(const SegmentationDataWrapper& sdw){
    POST(new OmSegmentEvent(OmSegmentEvent::SEGMENT_GUI_LIST, sdw));
}

void OmEvents::SegmentModified(){
    POST(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION));
}

void OmEvents::SegmentModified(boost::shared_ptr<OmSelectSegmentsParams> params){
    POST(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION, params));
}

void OmEvents::ViewCenterChanged(){
    POST(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
}

void OmEvents::View3dRecenter(){
    POST(new OmView3dEvent(OmView3dEvent::RECENTER));
}

void OmEvents::ViewBoxChanged(){
    POST(new OmViewEvent(OmViewEvent::VIEW_BOX_CHANGE));
}

void OmEvents::ViewPosChanged(){
    POST(new OmViewEvent(OmViewEvent::VIEW_POS_CHANGE));
}

void OmEvents::PreferenceChange(const int key){
    POST(new OmPreferenceEvent(OmPreferenceEvent::PREFERENCE_CHANGE, key));
}

#undef POST
