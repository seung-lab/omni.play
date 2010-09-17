#ifndef WATERSHED_LIB_RAW_CC_BINARY_H_
#define WATERSHED_LIB_RAW_CC_BINARY_H_

#include <boost/tuple/tuple.hpp>
#include <boost/unordered_map.hpp>
#include <vector>
#include <stdint.h>

struct Int64AndFloat {
	int64_t first;
	float second;
};

struct FloatAndInt64 {
	float first;
	int64_t second;
};

//typedef Int64AndFloat graph_t;
//typedef FloatAndInt64 dend_t;

typedef std::pair<int64_t, float> graph_t;
typedef std::pair<float, int64_t> dend_t;

void
rawQuickieWS(const float* connections,
             int64_t xDim, int64_t yDim, int64_t zDim,
             const float loThreshold,
             const float hiThreshold,
             const int   noThreshold,
             const float absLowThreshold,
             int*  result,
             std::vector<graph_t>& graph,
             std::vector<dend_t>& dendQueue,
             std::vector<int> &sizes);


#endif

