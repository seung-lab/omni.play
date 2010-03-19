/*
 * Definitions for volume library.
 *
 * Brett Warne - bwarne@mit.edu - 1/13/09
 */

#ifndef OM_VOLUME_TYPES_H
#define OM_VOLUME_TYPES_H

#include "common/omStd.h"
#include <vmmlib/vmmlib.h>

//coordinate frames
typedef vmml::Vector3<int> DataCoord;
typedef vmml::Vector3<float> NormCoord;
typedef vmml::Vector3<float> SpaceCoord;
typedef vmml::Vector2<int> ScreenCoord;

typedef vmml::AxisAlignedBoundingBox<int> DataBbox;
typedef vmml::AxisAlignedBoundingBox<float> NormBbox;
typedef vmml::AxisAlignedBoundingBox<float> SpaceBbox;

#endif
