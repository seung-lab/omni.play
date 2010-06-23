#ifndef OM_SEGMENT_GRAPH_H
#define OM_SEGMENT_GRAPH_H

#include "common/omCommon.h"
#include "segment/lowLevel/DynamicTreeContainer.h"
#include "segment/lowLevel/omSegmentListBySize.h"

class OmSegmentCacheImplLowLevel;

class OmSegmentGraph {
 public:
	OmSegmentGraph();
	~OmSegmentGraph();

	void initialize( OmSegmentCacheImplLowLevel * cache );

	bool graph_doesGraphNeedToBeRefreshed( const quint32 maxValue ); 

	OmSegID graph_getRootID( const OmSegID segID );
	void graph_cut( const OmSegID segID );
	void graph_join( const OmSegID childRootID, const OmSegID parentRootID );
	
	quint32 getNumTopLevelSegs();

	OmSegmentListBySize mRootListBySize;
	OmSegmentListBySize mValidListBySize;

 private:
	DynamicTreeContainer<OmSegID> * mGraph;
	void buildSegmentSizeLists( OmSegmentCacheImplLowLevel * );
};

#endif
