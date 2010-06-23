#ifndef OM_SEGMENT_GRAPH_H
#define OM_SEGMENT_GRAPH_H

#include "common/omCommon.h"
#include "segment/lowLevel/DynamicTreeContainer.h"
#include "segment/lowLevel/omSegmentListBySize.h"

class OmSegmentGraph {
 public:
	OmSegmentGraph();
	~OmSegmentGraph();

	void initialize( const quint32 maxValue );

	bool doesGraphNeedToBeRefreshed( const quint32 maxValue ); 

	OmSegID getRootID( const OmSegID segID );
	void cut( const OmSegID segID );
	void join( const OmSegID childRootID, const OmSegID parentRootID );
	
	quint32 getNumTopLevelSegs();

	OmSegmentListBySize mRootListBySize;

 private:
	DynamicTreeContainer<OmSegID> * mGraph;
};

#endif
