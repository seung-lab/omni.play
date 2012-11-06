#pragma once

#include <cstring>
#include "enum/enum.hpp"

namespace om {
namespace common {

enum ZeroMem {
    ZERO_FILL,
    DONT_ZERO_FILL
};
enum Blocking {
    BLOCKING,
    NON_BLOCKING
};
enum ShouldUpdateAsType{
    UPDATE_AS_TYPE,
    DONT_UPDATE_AS_TYPE
};
enum SetValid{
    SET_VALID,
    SET_NOT_VALID
};
enum AffinityGraph {
    NO_AFFINITY,
    X_AFFINITY,
    Y_AFFINITY,
    Z_AFFINITY
};
enum ShouldBufferWrites {
    BUFFER_WRITES,
    DONT_BUFFER_WRITES
};
enum AllowOverwrite {
    WRITE_ONCE,
    OVERWRITE
};
enum AddOrSubtract {
    ADD,
    SUBTRACT
};
enum ShouldForce {
    FORCE,
    DONT_FORCE
};
enum Direction {
    LEFT,
    RIGHT,
    UP,
    DOWN
};
enum Safe {
    SAFE,
    NOT_SAFE
};
// WARNING: used for serialization by OmSegmentPage,
//  and values should fit inside uint8_t
enum SegListType {
    WORKING = 0,
    VALID = 1,
    UNCERTAIN = 2
};
enum SegmentColorCacheType { SCC_FILTER_BLACK_BRIGHTEN_SELECT = 0,
                             SCC_FILTER_COLOR_BRIGHTEN_SELECT,
                             SCC_FILTER_BREAK,
                             SCC_FILTER_VALID,
                             SCC_FILTER_VALID_BLACK,
                             SCC_SEGMENTATION,
                             SCC_SEGMENTATION_BREAK_BLACK,
                             SCC_SEGMENTATION_BREAK_COLOR,
                             SCC_SEGMENTATION_VALID,
                             SCC_SEGMENTATION_VALID_BLACK,
                             SCC_FILTER_BLACK_DONT_BRIGHTEN_SELECT,
                             SCC_FILTER_COLOR_DONT_BRIGHTEN_SELECT,
                             SCC_NUMBER_OF_ENUMS };

enum CacheGroup {
    MESH_CACHE = 1,
    TILE_CACHE
};

inline std::ostream& operator<<(std::ostream &out, const CacheGroup& c)
{
    if(MESH_CACHE == c){
        out << "MESH_CACHE";
    } else {
        out << "TILE_CACHE";
    }
    return out;
}

BOOST_ENUM_VALUES(DataType, std::string,
                  (UNKNOWN)("unknown")
                  (INT8)("int8_t")
                  (UINT8)("uint8_t")
                  (INT32)("int32_t")
                  (UINT32)("uint32_t")
                  (FLOAT)("float"))

} // common
} // om

