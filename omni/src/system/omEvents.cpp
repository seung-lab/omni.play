#include "system/omEvent.h"
#include "system/omEvents.h"
#include "system/events/omSegmentEvent.h"
#include "system/events/omToolModeEvent.h"
#include "system/events/omViewEvent.h"
#include "system/omEventManager.h"

void OmEvents::ToolChange()
{
	OmEventManager::PostEvent(new OmToolModeEvent(OmToolModeEvent::TOOL_MODE_CHANGE));
}

void OmEvents::Redraw()
{
	OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
}

void OmEvents::SegmentModified()
{
	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION));
}
