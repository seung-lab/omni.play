#ifndef OM_SEGMENT_TYPES_H
#define OM_SEGMENT_TYPES_H

/*
 * Type definitions for segment library.
 *
 * Brett Warne - bwarne@mit.edu - 3/9/09
 */

#include "system/omSystemTypes.h"

static const OmId NULL_SEGMENT_ID = 0;

//internal storage size of segment data
#define SEGMENT_DATA_BYTES_PER_SAMPLE 4
#define SEGMENT_DATA_SAMPLES_PER_PIXEL 1

typedef unsigned int SEGMENT_DATA_TYPE;
typedef set < SEGMENT_DATA_TYPE > SegmentDataSet;

static const SEGMENT_DATA_TYPE NULL_SEGMENT_DATA = 0;

#endif
