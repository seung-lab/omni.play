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

	void drawRectangle(SpaceCoord v0,
			   SpaceCoord v1,
			   SpaceCoord v2,
			   SpaceCoord v3);

	void draw2dBox(const ViewType plane,
				   const Vector2f& min,
				   const Vector2f& max,
				   const float depth);

	void drawLines(SpaceCoord depth);

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
