#pragma once

#include "common/common.h"
#include "common/colors.h"

namespace om {
namespace segments {

struct data
{
    common::segId value;
    common::color color;
    uint64_t size;
    AxisAlignedBoundingBox<int> bounds;
};

struct edge
{
	common::segId seg1;
	common::segId seg2;
	double threshold;
};

}}
