#include "system/events/omPreferenceEvent.h"
#include "system/events/omSegmentEvent.h"
#include "system/events/omToolModeEvent.h"
#include "system/events/omView3dEvent.h"
#include "system/events/omViewEvent.h"
#include "system/events/omEvent.h"
#include "system/events/omEventManager.h"
#include "system/omEvents.h"

#define POST OmEventManager::PostEvent

void OmEvents::ToolChange(){
	POST(new OmToolModeEvent(OmToolModeEvent::TOOL_MODE_CHANGE));
}

void OmEvents::Redraw2d(){
	POST(new OmViewEvent(OmViewEvent::REDRAW));
}

void OmEvents::Redraw3d(){
	POST(new OmView3dEvent(OmView3dEvent::REDRAW));
}

void OmEvents::SegmentModified(){
	POST(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION));
}

void OmEvents::SegmentModified(OmID segmentationId,
							   const OmID segmentJustSelectedID,
							   void* sender,
							   std::string comment,
							   const bool doScroll )
{
	POST(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION,
							segmentationId,
							segmentJustSelectedID,
							sender,
							comment,
							doScroll));
}

void OmEvents::ViewCenterChanged(){
	POST(new OmViewEvent(OmViewEvent::VIEW_CENTER_CHANGE));
}

void OmEvents::ViewRecenter(){
	POST(new OmView3dEvent(OmView3dEvent::RECENTER));
}

void OmEvents::ViewBoxChanged(){
	POST(new OmViewEvent(OmViewEvent::VIEW_BOX_CHANGE));
}

void OmEvents::ViewPosChanged(){
	POST(new OmViewEvent(OmViewEvent::VIEW_POS_CHANGE));
}

void OmEvents::SegmentEditSelectionChanged(){
	POST(new OmSegmentEvent(OmSegmentEvent::SEGMENT_EDIT_SELECTION_CHANGE));
}

void OmEvents::PreferenceChange(const int key){
	POST(new OmPreferenceEvent(OmPreferenceEvent::PREFERENCE_CHANGE, key));
}

void OmEvents::View3dPreferenceChange(){
	POST(new OmView3dEvent(OmView3dEvent::UPDATE_PREFERENCES));
}

#undef POST
