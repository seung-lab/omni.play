#pragma once

#include "vmmlib/vmmlib.h"
using namespace vmml;

#include <iostream>
#include "common/macro.hpp"

namespace om {
namespace coords {

class DataBbox;
class Data;
class VolumeSystem;

class Chunk : public vmml::Vector3i {

public:
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

    Data ToData(const VolumeSystem*) const;
    DataBbox BoundingBox(const VolumeSystem*) const;
    uint64_t PtrOffset(const VolumeSystem*, int64_t) const;
    int SliceDepth(const VolumeSystem*, Global, om::common::ViewType) const;

    void operator=(const Chunk& rhs);
    bool operator==(const Chunk& rhs) const;
    bool operator!=(const Chunk& rhs) const;
    bool operator<(const Chunk& rhs) const;

private:
	typedef Vector3i base_t;
    PROP_CONST_REF(int, mipLevel);

    friend std::ostream& operator<<(std::ostream &out, const Chunk &in);
};

} // namespace coords
} // namespace om
