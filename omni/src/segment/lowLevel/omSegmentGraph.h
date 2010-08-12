#ifndef OM_SEGMENT_GRAPH_H
#define OM_SEGMENT_GRAPH_H

#include "common/omCommon.h"
#include "segment/lowLevel/DynamicForestPool.hpp"

class OmSegment;
class OmSegmentCacheImplLowLevel;
class OmSegmentLists;

class OmSegmentGraph {
 public:
	OmSegmentGraph();
	~OmSegmentGraph();

	void initialize( OmSegmentCacheImplLowLevel * cache );
	void growGraphIfNeeded(OmSegment * newSeg);

	bool graph_doesGraphNeedToBeRefreshed( const quint32 maxValue );
	OmSegID graph_getRootID( const OmSegID segID );
	void graph_cut( const OmSegID segID );
	void graph_join( const OmSegID childRootID, const OmSegID parentRootID );

	quint32 getNumTopLevelSegs();

	void setGlobalThreshold( const quint32 * dend,
				 const float * dendValues,
				 quint8 * edgeDisabledByUser,
				 quint8 * edgeWasJoined,
				 quint8 * edgeForceJoin,
				 const int size,
				 const float stopPoint);

	void resetGlobalThreshold( const quint32 * dend,
				   const float * dendValues,
				   quint8 * edgeDisabledByUser,
				   quint8 * edgeWasJoined,
				   quint8 * edgeForceJoin,
				   const int size,
				   const float stopPoint );

	void updateSizeListsFromJoin( OmSegment * root, OmSegment * child );
	void updateSizeListsFromSplit( OmSegment * parent, OmSegment * child );

	inline boost::shared_ptr<OmSegmentLists> getSegmentLists();

 private:

	zi::DynamicForestPool<uint32_t> * mGraph;
	OmSegmentCacheImplLowLevel * mCache;

	void buildSegmentSizeLists();

	bool JoinInternal( const OmSegID parentID, const OmSegID childUnknownDepthID,
			   const float threshold, const int edgeNumber);

	bool splitChildFromParentInternal( const OmSegID childID );

	quint64 computeSegmentSizeWithChildren(const OmSegID segID );
};

#endif
