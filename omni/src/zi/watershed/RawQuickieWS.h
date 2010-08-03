#ifndef WATERSHED_LIB_RAW_CC_BINARY_H_
#define WATERSHED_LIB_RAW_CC_BINARY_H_

#include <boost/tuple/tuple.hpp>
#include <boost/unordered_map.hpp>
#include <vector>
#include <stdint.h>

void
rawQuickieWS(const float* connections,
             int64_t xDim, int64_t yDim, int64_t zDim,
             const float loThreshold,
             const float hiThreshold,
             const int   noThreshold,
             const float absLowThreshold,
             int*  result,
             std::vector<std::pair<int64_t, float> >   &graph,
             std::vector<std::pair<float, int64_t> >   &dendQueue,
             std::vector<int> &sizes);


#endif

