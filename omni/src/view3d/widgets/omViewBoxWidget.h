#ifndef OM_VIEW_BOX_WIDGET_H
#define OM_VIEW_BOX_WIDGET_H

#include "view3d/omCamera.h"
#include "view3d/omView3dWidget.h"

class OmViewGroupState;

class OmViewBoxWidget : public OmView3dWidget {
public:
	OmViewBoxWidget(OmView3d *view3d, OmViewGroupState * vgs);
	virtual void Draw();

private:
	OmViewGroupState* mViewGroupState;

	void drawRectangle(DataCoord v0,
			   DataCoord v1,
			   DataCoord v2,
			   DataCoord v3);

	void draw2dBox(const ViewType plane,
				   const Vector2f& min,
				   const Vector2f& max,
				   const float depth);

	void drawLines(DataCoord depth);

	void drawChannelData(ViewType plane,
			     std::vector<OmTilePtr>);

	bool GetTextureMax(Vector3f coord,
			   ViewType plane,
			   Vector2f& dataMax,
			   Vector2f& spaceMax );

	bool GetTextureMin(Vector3f coord,
			   ViewType plane,
			   Vector2f & dataMin,
			   Vector2f & spaceMin);
};



#endif
