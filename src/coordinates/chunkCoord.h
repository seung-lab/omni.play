#pragma once

/*
 * chunkCoord represents a location in Mip Space given by a level (or mip resolution),
 *  and an x,y,z coordinate.  This is stored in a four element tuple as [level, (x,y,z)].
 *
 * Brett Warne - bwarne@mit.edu - 2/24/09
 */

#include "vmmlib/vmmlib.h"
using namespace vmml;

#include <iostream>

namespace om {
namespace coords {

class dataBbox;
class dataCoord;
class volumeSystem;

class chunkCoord {

public:
    chunkCoord();
    chunkCoord(int, const Vector3i &);
    chunkCoord(int level, int, int, int);

    //static const chunkCoord NULL_COORD;

    std::string getCoordsAsString() const;

    //property
    inline bool IsLeaf() const {
        return 0 == Level;
    }

    //family coordinate methods
    chunkCoord ParentCoord() const;
    chunkCoord PrimarySiblingCoord() const;
    void SiblingCoords(chunkCoord *pSiblings) const;
    chunkCoord PrimaryChildCoord() const;
    void ChildrenCoords(chunkCoord *pChildren) const;

    //access
    inline int getLevel() const {
        return Level;
    }
    inline int X() const {
        return Coordinate.x;
    }
    inline int Y() const {
        return Coordinate.y;
    }
    inline int Z() const {
        return Coordinate.z;
    }

    dataCoord toDataCoord(const volumeSystem*) const;
    dataBbox chunkBoundingBox(const volumeSystem*) const;
    uint64_t chunkPtrOffset(const volumeSystem*, int64_t) const;
    int sliceDepth(const volumeSystem*, globalCoord, common::viewType) const;

    //operators
    void operator=(const chunkCoord& rhs);
    bool operator==(const chunkCoord& rhs) const;
    bool operator!=(const chunkCoord& rhs) const;
    bool operator<(const chunkCoord& rhs) const;

    int Level;
    Vector3i Coordinate;

    friend std::ostream& operator<<(std::ostream &out, const chunkCoord &in);
};

} // namespace coords
} // namespace om
