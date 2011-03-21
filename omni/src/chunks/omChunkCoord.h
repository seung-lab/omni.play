#ifndef OM_MIP_CHUNK_COORD_H
#define OM_MIP_CHUNK_COORD_H

/*
 * OmChunkCoord represents a location in Mip Space given by a level (or mip resolution),
 *  and an x,y,z coordinate.  This is stored in a four element tuple as [level, (x,y,z)].
 *
 * Brett Warne - bwarne@mit.edu - 2/24/09
 */

#include "common/omCommon.h"
#include <iostream>

class OmChunkCoord {

public:
    OmChunkCoord();
    OmChunkCoord(int, const DataCoord &);
    OmChunkCoord(int level, int, int, int);

    static const OmChunkCoord NULL_COORD;

    QString getCoordsAsString() const;

    //property
    inline bool IsLeaf() const {
        return 0 == Level;
    }

    //family coordinate methods
    OmChunkCoord ParentCoord() const;
    OmChunkCoord PrimarySiblingCoord() const;
    void SiblingCoords(OmChunkCoord *pSiblings) const;
    OmChunkCoord PrimaryChildCoord() const;
    void ChildrenCoords(OmChunkCoord *pChildren) const;

    //access
    inline int getLevel() const {
        return Level;
    }
    inline int getCoordinateX() const {
        return Coordinate.x;
    }
    inline int getCoordinateY() const {
        return Coordinate.y;
    }
    inline int getCoordinateZ() const {
        return Coordinate.z;
    }

    //operators
    void operator=(const OmChunkCoord& rhs);
    bool operator==(const OmChunkCoord& rhs) const;
    bool operator!=(const OmChunkCoord& rhs) const;
    bool operator<(const OmChunkCoord& rhs) const;

    int Level;
    DataCoord Coordinate;

    friend std::ostream& operator<<(std::ostream &out, const OmChunkCoord &in);
};

#endif
