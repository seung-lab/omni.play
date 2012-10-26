#pragma once

#include "vmmlib/vmmlib.h"
using namespace vmml;

#include <iostream>
#include "common/macro.hpp"

namespace om {
namespace coords {

class dataBbox;
class data;
class volumeSystem;

class Chunk {

public:
    Chunk();
    Chunk(int, const Vector3i &);
    Chunk(int level, int, int, int);

    std::string GetCoordsAsString() const;

    inline bool IsLeaf() const {
        return 0 == mipLevel_;
    }

    //family coordinate methods
    Chunk ParentCoord() const;
    Chunk PrimarySiblingCoord() const;
    std::vector<Chunk> SiblingCoords() const;
    Chunk PrimaryChildCoord() const;
    std::vector<Chunk> ChildrenCoords() const;

    data ToData(const volumeSystem*) const;
    dataBbox BoundingBox(const volumeSystem*) const;
    uint64_t PtrOffset(const volumeSystem*, int64_t) const;
    int SliceDepth(const volumeSystem*, global, common::viewType) const;

    void operator=(const Chunk& rhs);
    bool operator==(const Chunk& rhs) const;
    bool operator!=(const Chunk& rhs) const;
    bool operator<(const Chunk& rhs) const;

private:
    PROP_CONST_REF(int, mipLevel);
    PROP_CONST_REF(Vector3i, coord);

    friend std::ostream& operator<<(std::ostream &out, const Chunk &in);
};

} // namespace coords
} // namespace om
