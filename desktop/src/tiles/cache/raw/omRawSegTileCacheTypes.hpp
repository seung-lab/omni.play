#pragma once

#include <memory>

template <class> class OmPooledTile;

typedef std::shared_ptr<OmPooledTile<uint32_t> > PooledTile32Ptr;
