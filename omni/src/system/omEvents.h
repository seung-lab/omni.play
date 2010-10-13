#ifndef OM_EVENTS_H
#define OM_EVENTS_H

class OmEvents {
 public:
	static void ToolChange();
	static void Redraw();
	static void Redraw3d();
	static void SegmentModified();
	static void ViewCenterChanged();
	static void ViewBoxChanged();
	static void ViewPosChanged();
	static void SegmentEditSelectionChanged();
	static void PreferenceChange(const int key);
};

#endif
