#include "segment/lowLevel/omSegmentGraph.h"

OmSegmentGraph::OmSegmentGraph()
	: mGraph(NULL)
{
}

OmSegmentGraph::~OmSegmentGraph()
{
	delete mGraph;
}

void OmSegmentGraph::cut( const OmSegID segID )
{
	mGraph->get(segID)->cut();
}

OmSegID OmSegmentGraph::getRootID( const OmSegID segID )
{
	return mGraph->get(segID)->findRoot()->getKey();
}

void OmSegmentGraph::join( const OmSegID childRootID, const OmSegID parentRootID )
{
	mGraph->get(childRootID)->join(mGraph->get(parentRootID));
}

bool OmSegmentGraph::doesGraphNeedToBeRefreshed( const quint32 maxValue )
{
	return (NULL == mGraph || mGraph->getSize() != maxValue+1 );
}

void OmSegmentGraph::initializeDynamicTree( const quint32 maxValue )
{
	delete mGraph;

	// maxValue is a valid segment id, so array needs to be 1 bigger
	const int size = maxValue + 1;
	
	mGraph = new DynamicTreeContainer<OmSegID>( size );
}
