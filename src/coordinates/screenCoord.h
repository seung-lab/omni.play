#pragma once

#include "vmmlib/vmmlib.h"

namespace om {
namespace coords {
    
class globalCoord;
class screenSystem;
    
class screenCoord : public vmml::Vector2i 
{
private:
    typedef vmml::Vector2i base_t;
    const screenSystem * system_;
    
public:
    screenCoord(base_t v, const screenSystem * system) 
        : base_t(v) 
        , system_(system)
    { }
    
    screenCoord(int x, int y, const screenSystem * system)
        : base_t(x, y)
        , system_(system)
    { }
    
    screenCoord(const screenCoord& coord)
        : base_t(coord)
        , system_(coord.system_)
    { }
    
    globalCoord toGlobalCoord() const ;
};

} // namespace coords
} // namespace om