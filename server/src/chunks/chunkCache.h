#pragma once

#include <boost/unordered_map.hpp>

#include "chunks/chunk.hpp"

namespace om {
namespace chunks {

template <typename T> class cache {
 private:
  boost::unordered_map<Chunk<T> > public : private:
};

}
}  // namespace om::chunks::
