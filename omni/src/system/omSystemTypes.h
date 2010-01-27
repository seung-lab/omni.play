#ifndef OM_SYSTEM_TYPES_H
#define OM_SYSTEM_TYPES_H

/*
 * Type definitions for system library.
 *
 * Brett Warne - bwarne@mit.edu - 1/13/09
 */

#include "common/omStd.h"

//id typedefs
typedef uint32_t OmId;
typedef set < OmId > OmIds;

const static OmId NULL_OM_ID = 0;

//bit field
typedef unsigned int OmBitfield;

//slices
enum ViewType { XY_VIEW, XZ_VIEW, YZ_VIEW, ALL };

//objects
enum ObjectType { VOLUME, CHANNEL, SEGMENTATION, SEGMENT, NOTE, FILTER };

#endif
