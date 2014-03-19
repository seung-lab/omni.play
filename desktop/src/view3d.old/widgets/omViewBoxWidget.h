#pragma once
#include "precomp.h"

#include "tiles/omTileTypes.hpp"
#include "../omCamera.h"
#include "../omView3dWidget.h"

class OmViewGroupState;
class OmViewGroupView2dState;

class OmViewBoxWidget : public OmView3dWidget {
 public:
  OmViewBoxWidget(OmView3d* view3d, OmViewGroupState& vgs);
  virtual void Draw();

 private:
  OmViewGroupState& vgs_;

  void drawRectangle(const Vector3i& v0, const Vector3i& v1, const Vector3i& v2,
                     const Vector3i& v3);

  void draw2dBox(const om::common::ViewType plane, const Vector2f& min,
                 const Vector2f& max, const float depth);

  void drawLines(Vector3i depth);

  //     void drawChannelData(ViewType plane,
  //                          std::vector<OmTilePtr>);
  //
  //     bool getTextureMax(Vector3f coord,
  //                        om::common::ViewType plane,
  //                        Vector2f& dataMax,
  //                        Vector2f& spaceMax );
  //
  //     bool getTextureMin(Vector3f coord,
  //                        om::common::ViewType plane,
  //                        Vector2f & dataMin,
  //                        Vector2f & spaceMin);

  void draw2dBoxWrapper(OmViewGroupView2dState& view2dState,
                        const om::common::ViewType viewType);
};
