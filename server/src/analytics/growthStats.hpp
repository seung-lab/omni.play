#pragma once

#include <vector>
#include <utility>
#include <algorithm>
#include <zi/disjoint_sets/disjoint_sets.hpp>
#include <zi/utility/for_each.hpp>
#include <zi/utility/assert.hpp>

#include "segment/segmentTypes.h"
#include "segment/mstTypes.h"

namespace om {
namespace analytics {

inline void get_growth(std::vector<std::pair<double, uint64_t>>* _return,
                       const uint32_t& seed,
                       const std::vector<segment::Data>& data,
                       const std::vector<segments::edge>& mst_in) {
  std::vector<segments::mstEdge> mst(mst_in);
  std::sort(mst.begin(), mst.end());

  zi::disjoint_sets<uint32_t> sets(data.size());

  _return.clear();
  _return.push_back(std::make_pair(static_cast<double>(1), data[seed].size));

  FOR_EACH_R(it, mst) {
    uint32_t id1 = sets.find_set(it->seg1);
    uint32_t id2 = sets.find_set(it->seg2);
    if (id1 != id2) {
      if (id2 == seed) {
        std::swap(id1, id2);
      }

      if (id1 == seed) {
        ZI_ASSERT(id2 == it->seg1 || id2 == it->seg2);

        if (it->threshold == _return.back().first) {
          _return.back().second += data[id2].size;
        } else {
          uint64_t new_size = _return.back().second + data[id2].size;
          _return.push_back(std::make_pair(it->threshold, new_size));
        }
        seed = sets.join(id1, id2);
      }
    }
  }
}

}  // namespace analytics
}  // namespace om
