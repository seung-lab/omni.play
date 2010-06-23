#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/lowLevel/omSegmentIteratorLowLevel.h"

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

void OmSegmentGraph::initialize( const quint32 maxValue )
{
	delete mGraph;
		
	// maxValue is a valid segment id, so array needs to be 1 bigger
	const quint32 size = maxValue + 1;
	
	mGraph = new DynamicTreeContainer<OmSegID>( size );
}

quint32 OmSegmentGraph::getNumTopLevelSegs()
{
	return mRootListBySize.size();
}

void OmSegmentGraph::buildSegmentSizeLists( OmSegmentCacheImplLowLevel * cache )
{
	mValidListBySize.clear();
	mRootListBySize.clear();

	OmSegmentIteratorLowLevel iter(cache);
	iter.iterOverAllSegments();

	for(OmSegment * seg = iter.getNextSegment(); NULL != seg; seg = iter.getNextSegment()){
		if(0 == seg->mParentSegID) {
			if(seg->mImmutable) {
				mValidListBySize.insertSegment( seg );
			} else {
				mRootListBySize.insertSegment( seg );
			}
		} 
	}
}
