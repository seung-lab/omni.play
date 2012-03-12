#pragma once

namespace om {
namespace segments {

struct mstEdge {
    uint32_t number;
    uint32_t node1ID;
    uint32_t node2ID;
    double threshold;
    uint8_t userJoin;
    uint8_t userSplit;
    uint8_t wasJoined; // transient state

    bool operator<(mstEdge const & rhs) const
    {
        return threshold < rhs.threshold;
    }
};

}}
