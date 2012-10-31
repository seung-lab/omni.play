#pragma once

#include "vmmlib/vmmlib.h"

namespace om {
namespace coords {

class Global;
class screenSystem;

class Screen : public vmml::Vector2i
{
private:
    typedef vmml::Vector2i base_t;

public:
    Screen(base_t v, const screenSystem * system)
        : base_t(v)
        , system_(system)
    { }

    Screen(int x, int y, const screenSystem * system)
        : base_t(x, y)
        , system_(system)
    { }

    Screen(const Screen& coord)
        : base_t(coord)
        , system_(coord.system_)
    { }

    Global ToGlobal() const;
private:
	PROP_CONST_PTR(screenSystem, system);
};

} // namespace coords
} // namespace om
