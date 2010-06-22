#ifndef OM_SEGMENT_GRAPH_H
#define OM_SEGMENT_GRAPH_H

#include "common/omCommon.h"
#include "segment/DynamicTreeContainer.h"

class OmSegmentGraph {
 public:
	OmSegmentGraph();
	~OmSegmentGraph();

	void initializeDynamicTree( const quint32 maxValue );

	bool doesGraphNeedToBeRefreshed( const quint32 maxValue ); 

	OmSegID getRootID( const OmSegID segID );
	void cut( const OmSegID segID );
	void join( const OmSegID childRootID, const OmSegID parentRootID );

 private:
	DynamicTreeContainer<OmSegID> * mGraph;
};

#endif
