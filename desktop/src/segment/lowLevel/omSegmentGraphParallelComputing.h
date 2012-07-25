#include "segment/io/omMSTtypes.h"
#include "segment/io/omMST.h"
#include "segment/lowLevel/omSegmentGraph.h"

class OmSegment;
class OmMST;
class OmSegmentGraph;

class OmSegmentGraphParallelComputing
{
	private:

		struct point
    	{
        	int x,y,z;
    	};

		static const int step                    = 100;
    	static const int max_nos                 = 10 ;

    	int step_;

    	std::vector <OmMSTEdge*> edgesInBatch[2][4000];
	public:
		int encodeBatch(point batch_);
		int checkBatch(OmSegment *segment, int _step);
		void HandleInitialBatches(bool flag, boost::unordered_map <OmSegID,OmSegment*> *accessToSegments_, int batchID, OmSegmentGraph *graph_, OmMST *mst);
		void MergeBatches(OmSegmentGraph *graph_, boost::unordered_map <OmSegID,OmSegment*> *accessToSegments_, OmSegmentGraph::some_type* distribution_, OmMST *mst, OmSegmentGraph::AdjacencyMap* AdjacencyList);

};