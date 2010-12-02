#ifndef OM_SEGMENT_GRAPH_H
#define OM_SEGMENT_GRAPH_H

#include "common/omCommon.h"
#include "segment/lowLevel/DynamicForestPool.hpp"

class OmSegment;
class OmSegmentCacheImplLowLevel;
class OmSegmentLists;
class OmMST;
class OmValidGroupNum;

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

	void setGlobalThreshold(boost::shared_ptr<OmMST> mst);
	void resetGlobalThreshold(boost::shared_ptr<OmMST> mst);

	void updateSizeListsFromJoin( OmSegment * root, OmSegment * child );
	void updateSizeListsFromSplit( OmSegment * parent, OmSegment * child );

	inline boost::shared_ptr<OmSegmentLists> getSegmentLists();

private:

	zi::DynamicForestPool<uint32_t> * mGraph;
	OmSegmentCacheImplLowLevel * mCache;

	void buildSegmentSizeLists();

	bool JoinInternal( const OmSegID parentID, const OmSegID childUnknownDepthID,
					   const double threshold, const int edgeNumber);

	bool splitChildFromParentInternal( const OmSegID childID );

	quint64 computeSegmentSizeWithChildren(const OmSegID segID );
	boost::shared_ptr<OmValidGroupNum>& GetValidGroupNum() const;
};

#endif
