#ifndef OM_VOL_SLICE_CACHE_TYPES_HPP
#define OM_VOL_SLICE_CACHE_TYPES_HPP

#include "utility/omSharedPtr.hpp"

template <class> class OmPooledTile;

typedef om::shared_ptr<OmPooledTile<uint32_t> > PooledTile32Ptr;

#endif
