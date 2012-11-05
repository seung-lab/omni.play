#pragma once

#include "common/omStd.h"
#include "utility/omSharedPtr.hpp"

template <class> class OmPooledTile;

typedef boost::shared_ptr<OmPooledTile<uint32_t> > PooledTile32Ptr;

