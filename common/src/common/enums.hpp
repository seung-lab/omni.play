#pragma once

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
enum ShouldThrottle {
    THROTTLE,
    DONT_THROTTLE
};
enum ShouldFifo {
    FIFO,
    DONT_FIFO
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
enum ReadOrReread {
    READ,
    REREAD
};
// WARNING: used for serialization by OmSegmentPage,
//  and values should fit inside uint8_t
enum SegListType {
    WORKING = 0,
    VALID = 1,
    UNCERTAIN = 2
};

} // common
} // om

