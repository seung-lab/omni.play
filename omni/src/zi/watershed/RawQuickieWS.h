#ifndef WATERSHED_LIB_RAW_CC_BINARY_H_
#define WATERSHED_LIB_RAW_CC_BINARY_H_

#include <boost/tuple/tuple.hpp>
#include <boost/unordered_map.hpp>
#include <vector>
#include <stdint.h>

#include <zi/utility>

#include "utility/omSmartPtr.hpp"

/*
struct Int64AndFloat {
	int64_t first;
	float second;
};

struct FloatAndInt64 {
	float first;
	int64_t second;
};

typedef Int64AndFloat graph_t;
typedef FloatAndInt64 dend_t;
*/

typedef std::pair<int64_t, float> graph_t;
typedef std::pair<float, int64_t> dend_t;

typedef boost::tuple<boost::shared_ptr<int>,
		     boost::shared_ptr<float>,
		     size_t> RawQuickieWStreeOutput;

class RawQuickieWS {
public:
	RawQuickieWS(int64_t xDim, int64_t yDim, int64_t zDim,
		     const float loThreshold,
		     const float hiThreshold,
		     int   noThreshold,
		     const float absLowThreshold)
		: xDim(xDim)
		, yDim(yDim)
		, zDim(zDim)
		, loThreshold(loThreshold)
		, hiThreshold(hiThreshold)
		, noThreshold(noThreshold)
		, absLowThreshold(absLowThreshold)
	{}

	RawQuickieWStreeOutput run(const float* connections,
				   int* result)
	{
		rawQuickieWS(connections, result);

		return boost::make_tuple(computeMSTedges(),
					 computeMSTthresholds(),
					 dendQueue.size());
	}

private:
	int64_t xDim;
	int64_t yDim;
	int64_t zDim;
	const float loThreshold;
	const float hiThreshold;
	const int   noThreshold;
	const float absLowThreshold;

	std::vector<graph_t> graph;
	std::vector<dend_t> dendQueue;
	std::vector<int> sizes;

	void rawQuickieWS(const float* connections,
			  int*  result);

	/*
	void computeRegionGraphEdges()
	{
		DECLARE_MEX_RET_ARRAY(retGraph, plhs[1], int, graph.size(), 2);
		int idx = 0;
		FOR_EACH (it, graph) retGraphData[idx++] = (it->first >> 32);
		FOR_EACH (it, graph) retGraphData[idx++] = (it->first & 0xFFFFFFFF);
	}

	void computeRegionGraphThresholds(const vector<graph_t> &graph,
					  mxArray *plhs[])
	{
		DECLARE_MEX_RET_ARRAY(retGraphVals, plhs[2], float, graph.size(), 1);
		int idx = 0;
		FOR_EACH (it, graph) retGraphValsData[idx++] = (float)it->second;
	}
	*/

	boost::shared_ptr<int> computeMSTedges()
	{
		std::cout << "computing MST edges..." << std::flush;

		boost::shared_ptr<int> retDendDataPtr =
			OmSmartPtr<int>::makeMallocPtrNumElements(dendQueue.size()*2);
		int* retDendData = retDendDataPtr.get();

		size_t idx = 0;
		FOR_EACH (it, dendQueue) retDendData[idx++] = (it->second >> 32);
		FOR_EACH (it, dendQueue) retDendData[idx++] = (it->second & 0xFFFFFFFF);

		std::cout << "done!\n";

		return retDendDataPtr;
	}

	boost::shared_ptr<float> computeMSTthresholds()
	{
		std::cout << "computing MST thresholds..." << std::flush;

		boost::shared_ptr<float> retDendValsDataPtr =
			OmSmartPtr<float>::makeMallocPtrNumElements(dendQueue.size());
		float* retDendValsData = retDendValsDataPtr.get();

		size_t idx = 0;
		FOR_EACH (it, dendQueue) retDendValsData[idx++] = (float)it->first;

		std::cout << "done!\n";

		return retDendValsDataPtr;
	}
};

#endif

