#pragma once

#include "common/common.h"
#include "common/colors.h"

namespace om {
namespace segment {

struct data {
    common::segId value;
    common::color color;
    uint64_t size;
    AxisAlignedBoundingBox<int> bounds;
};

}
}
