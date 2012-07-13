#pragma once

#include "common/omStd.h"

struct OmMSTEdge {
    uint32_t number;
    uint32_t node1ID;
    uint32_t node2ID;
    double threshold;
    uint8_t userJoin;
    uint8_t userSplit;
    uint8_t wasJoined; // transient state

    bool operator< (const OmMSTEdge &edge)
    {
        if ( threshold > edge.threshold ) return true;            
        if ( threshold == edge.threshold )
        {
            if ( node1ID < edge.node1ID ) return true;
            if ( node1ID == edge.node1ID && node2ID < edge.node2ID ) return true;
            return false;
        }
        return false;
    }
};

struct OmMSTImportEdge
{
    uint32_t node1ID;
    uint32_t node2ID;
    float threshold;
};
std::ostream& operator<<(std::ostream&, const OmMSTImportEdge&);

