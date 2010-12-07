#ifndef OM_EVENTS_H
#define OM_EVENTS_H

#include "common/omCommon.h"

class SegmentDataWrapper;

class OmEvents {
 public:
	static void ToolChange();
	static void Redraw2d();
	static void Redraw3d();
	static void SegmentModified();
	static void SegmentModified(const SegmentDataWrapper& sdw,
								void* sender,
								std::string comment,
								const bool doScroll);
	static void ViewCenterChanged();
	static void View3dRecenter();
	static void ViewBoxChanged();
	static void ViewPosChanged();
	static void SegmentEditSelectionChanged();
	static void PreferenceChange(const int key);
	static void View3dPreferenceChange();
};

#endif
