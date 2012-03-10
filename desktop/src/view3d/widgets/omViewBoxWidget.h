#pragma once

#include "tiles/omTileTypes.hpp"
#include "view3d/omCamera.h"
#include "view3d/omView3dWidget.h"

class OmViewGroupState;
class OmViewGroupView2dState;

class OmViewBoxWidget : public OmView3dWidget {
public:
    OmViewBoxWidget(OmView3d *view3d, OmViewGroupState* vgs);
    virtual void Draw();

private:
    OmViewGroupState* vgs_;

    void drawRectangle(const DataCoord& v0,
                       const DataCoord& v1,
                       const DataCoord& v2,
                       const DataCoord& v3);

    void draw2dBox(const ViewType plane,
                   const Vector2f& min,
                   const Vector2f& max,
                   const float depth);

    void drawLines(DataCoord depth);

    void drawChannelData(ViewType plane,
                         std::vector<OmTilePtr>);

    bool getTextureMax(Vector3f coord,
                       ViewType plane,
                       Vector2f& dataMax,
                       Vector2f& spaceMax );

    bool getTextureMin(Vector3f coord,
                       ViewType plane,
                       Vector2f & dataMin,
                       Vector2f & spaceMin);

    void draw2dBoxWrapper(OmViewGroupView2dState* view2dState,
                          const ViewType viewType);

};

