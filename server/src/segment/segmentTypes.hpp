#pragma once

#include "common/common.h"
#include "common/colors.h"

namespace om {
namespace segments {

struct data
{
    common::SegID value;
    common::Color color;
    uint64_t size;
    AxisAlignedBoundingBox<int> bounds;
};

struct edge
{
	common::SegID seg1;
	common::SegID seg2;
	double threshold;
};

}}
