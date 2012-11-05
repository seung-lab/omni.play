#pragma once

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

BOOST_ENUM_VALUES(DataType, std::string,
                  (UNKNOWN)("unknown")
                  (INT8)("int8_t")
                  (UINT8)("uint8_t")
                  (INT32)("int32_t")
                  (UINT32)("uint32_t")
                  (FLOAT)("float"))

} // common
} // om

