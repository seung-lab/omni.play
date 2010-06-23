#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/lowLevel/omSegmentIteratorLowLevel.h"
#include "segment/lowLevel/omSegmentCacheImplLowLevel.h"

OmSegmentGraph::OmSegmentGraph()
	: mGraph(NULL)
	, mCache(NULL)
{
}

OmSegmentGraph::~OmSegmentGraph()
{
	delete mGraph;
}

void OmSegmentGraph::graph_cut( const OmSegID segID )
{
	mGraph->get(segID)->cut();
}

OmSegID OmSegmentGraph::graph_getRootID( const OmSegID segID )
{
	return mGraph->get(segID)->findRoot()->getKey();
}

void OmSegmentGraph::graph_join( const OmSegID childRootID, const OmSegID parentRootID )
{
	mGraph->get(childRootID)->join(mGraph->get(parentRootID));
}

bool OmSegmentGraph::graph_doesGraphNeedToBeRefreshed( const quint32 maxValue )
{
	return (NULL == mGraph || mGraph->getSize() != maxValue+1 );
}

void OmSegmentGraph::initialize( OmSegmentCacheImplLowLevel * cache )
{
	mCache = cache;

	delete mGraph;
		
	// maxValue is a valid segment id, so array needs to be 1 bigger
	const quint32 size = 1 + mCache->getMaxValue();
	
	mGraph = new DynamicTreeContainer<OmSegID>( size );

	buildSegmentSizeLists();
}

void OmSegmentGraph::buildSegmentSizeLists()
{
	mValidListBySize.clear();
	mRootListBySize.clear();

	OmSegmentIteratorLowLevel iter(mCache);
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

quint32 OmSegmentGraph::getNumTopLevelSegs()
{
	return mRootListBySize.size() + mValidListBySize.size();
}


// TODO: store more threshold info in the segment cache, and reduce size of walk...
// NOTE: assuming incoming data is an edge list
void OmSegmentGraph::setGlobalThreshold( const quint32 * nodes, 
					 const float * thresholds, 
					 quint8 * edgeDisabledByUser,
					 quint8 * edgeWasJoined,
					 quint8 * edgeForceJoin,
					 const int numEdges, 
					 const float stopThreshold )
{
	printf("\t %d edges...", numEdges);
	fflush(stdout);

	OmSegID childID;
	OmSegID parentID;
	float threshold;

	for(int i = 0; i < numEdges; ++i) {
		if( 1 == edgeDisabledByUser[i] ){
			continue;
		}

		childID = nodes[i];
		threshold = thresholds[i];
		
		if( threshold >= stopThreshold ||
		    1 == edgeForceJoin[i] ){ // join
			if( 1 == edgeWasJoined[i] ){
				continue;
			}
			parentID = nodes[i + numEdges ];
			if( JoinInternal( parentID, childID, threshold, i) ){
				edgeWasJoined[i] = 1;
			} else {
				edgeDisabledByUser[i] = 1;
			}
		} else { // split
			if( 0 == edgeWasJoined[i] ){
				continue;
			}
			if( splitChildFromParentInternal( childID ) ){
				edgeWasJoined[i] = 0;
			} else {
				edgeForceJoin[i] = 1;
			}
		}
        }

	printf("done\n");
}

void OmSegmentGraph::resetGlobalThreshold( const quint32 * nodes, 
					   const float * thresholds, 
					   quint8 * edgeDisabledByUser,
					   quint8 * edgeWasJoined,
					   quint8 * edgeForceJoin,
					   const int numEdges, 
					   const float stopThreshold )
{
	printf("\t %d edges...", numEdges);
	fflush(stdout);

	OmSegID childID;
	OmSegID parentID;
	float threshold;

	for(int i = 0; i < numEdges; ++i) {
		if( 1 == edgeDisabledByUser[i] ){
			continue;
		}

		childID = nodes[i];
		threshold = thresholds[i];
		
		if( threshold >= stopThreshold ||
		    1 == edgeForceJoin[i] ){ // join
			if( 1 == edgeWasJoined[i] ){
				continue;
			}
			parentID = nodes[i + numEdges ];
			if( JoinInternal( parentID, childID, threshold, i) ){
				edgeWasJoined[i] = 1;
			} else {
				edgeDisabledByUser[i] = 1;
			}
		} else { // split
			if( 0 == edgeWasJoined[i] ){
				continue;
			}
			if( splitChildFromParentInternal( childID ) ){
				edgeWasJoined[i] = 0;
			} else {
				edgeForceJoin[i] = 1;
			}
		}
        }

	printf("done\n");
}

bool OmSegmentGraph::JoinInternal( const OmSegID parentID, 
				   const OmSegID childUnknownDepthID, 
				   const float threshold,
				   const int edgeNumber )
{
	const OmSegID childRootID = graph_getRootID(childUnknownDepthID);
	OmSegment * childRoot = mCache->GetSegmentFromValue(childRootID);
	OmSegment * parent = mCache->GetSegmentFromValue( parentID );

	if( childRoot == mCache->findRoot( parent ) ){
		return false;
	}
	
	if( childRoot->mImmutable != parent->mImmutable ){
		return false;
	}
 
	graph_join(childRootID, parentID);

	parent->segmentsJoinedIntoMe.insert( childRoot->mValue );
	childRoot->setParent(parent, threshold);
	childRoot->mEdgeNumber = edgeNumber;

	updateSizeListsFromJoin( parent, childRoot );

	return true;
}

bool OmSegmentGraph::splitChildFromParentInternal( const OmSegID childID )
{
	OmSegment * child = mCache->GetSegmentFromValue( childID );

	if( child->mThreshold > 1 ){
		return false;
	}

	assert( child->mParentSegID );

	OmSegment * parent = mCache->GetSegmentFromValue( child->mParentSegID );

	if( child->mImmutable == parent->mImmutable &&
	    1 == child->mImmutable ){
		return false;
	}
	
	parent->segmentsJoinedIntoMe.erase( child->mValue );
        graph_cut(child->mValue);
	child->mParentSegID = 0;
	child->mEdgeNumber = -1;

	return true;
}

void OmSegmentGraph::updateSizeListsFromJoin( OmSegment * parent, OmSegment * child )
{
	OmSegment * root = mCache->findRoot(parent);
	mRootListBySize.updateFromJoin( root, child );
	mValidListBySize.updateFromJoin( root, child );
}
