#pragma once

#include "utility/omSharedPtr.hpp"

template <class> class OmPooledTile;

typedef std::shared_ptr<OmPooledTile<uint32_t> > PooledTile32Ptr;

