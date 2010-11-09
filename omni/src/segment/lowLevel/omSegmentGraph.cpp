#include "segment/io/omMST.h"
#include "utility/stringHelpers.h"
#include "volume/omSegmentation.h"
#include "segment/omSegmentLists.hpp"
#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/lowLevel/omSegmentIteratorLowLevel.h"
#include "segment/lowLevel/omSegmentCacheImplLowLevel.h"
#include "utility/omTimer.h"

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
	mGraph->cut(segID);
}

OmSegID OmSegmentGraph::graph_getRootID( const OmSegID segID )
{
	return mGraph->root(segID);
}

void OmSegmentGraph::graph_join( const OmSegID childRootID, const OmSegID parentRootID )
{
	mGraph->join(childRootID, parentRootID);
}

bool OmSegmentGraph::graph_doesGraphNeedToBeRefreshed( const quint32 maxValue )
{
	return (NULL == mGraph || mGraph->size() != maxValue+1 );
}

void OmSegmentGraph::initialize( OmSegmentCacheImplLowLevel * cache )
{
	mCache = cache;

	delete mGraph;

	// maxValue is a valid segment id, so array needs to be 1 bigger
	const quint32 size = 1 + mCache->getMaxValue();

	mGraph = new zi::DynamicForestPool<uint32_t>( size );

	buildSegmentSizeLists();
}

void OmSegmentGraph::growGraphIfNeeded(OmSegment * seg)
{
	// maxValue is a valid segment id, so array needs to be 1 bigger
	const quint32 size = 1 + mCache->getMaxValue();
	mGraph->resize(size);
	getSegmentLists()->InsertSegmentWorking(seg);
}

void OmSegmentGraph::buildSegmentSizeLists()
{
	getSegmentLists()->Valid().clear();
	getSegmentLists()->Working().clear();

	OmSegmentIteratorLowLevel iter(mCache);
	iter.iterOverAllSegments();

	for(OmSegment * seg = iter.getNextSegment();
		NULL != seg;
		seg = iter.getNextSegment())
	{
		if(!seg->getParent()) {
			switch(seg->GetListType()){
			case om::WORKING:
				getSegmentLists()->InsertSegmentWorking(seg);
				break;
			case om::VALID:
				getSegmentLists()->InsertSegmentValid(seg);
				break;
			case om::UNCERTAIN:
				getSegmentLists()->InsertSegmentUncertain(seg);
				break;
			default:
				throw OmArgException("unsupprted list arg");
			}
		}
	}
}

quint32 OmSegmentGraph::getNumTopLevelSegs()
{
	return getSegmentLists()->GetNumTopLevelSegs();
}

void OmSegmentGraph::setGlobalThreshold(boost::shared_ptr<OmMST> mst)
{
	printf("\t %s edges...",
		   StringHelpers::commaDeliminateNum(mst->NumEdges()).c_str());
	fflush(stdout);

	OmTimer timer;

	const double stopThreshold = mst->UserThreshold();
	OmMSTEdge* edges = mst->Edges();

	for(uint32_t i = 0; i < mst->NumEdges(); ++i) {
		if( 1 == edges[i].userSplit ){
			continue;
		}

		if( edges[i].threshold >= stopThreshold ||
		    1 == edges[i].userJoin){ // join
			if( 1 == edges[i].wasJoined ){
				continue;
			}

			if( JoinInternal( edges[i].node2ID,
							  edges[i].node1ID,
							  edges[i].threshold,
							  i) ){
				edges[i].wasJoined = 1;
			} else {
				edges[i].userSplit = 1;
			}
		}
	}

	printf("done (%f secs)\n", timer.s_elapsed() );
}

void OmSegmentGraph::resetGlobalThreshold(boost::shared_ptr<OmMST> mst)
{
	printf("\t %d edges...", mst->NumEdges());
	fflush(stdout);

	const double stopThreshold = mst->UserThreshold();
	OmMSTEdge* edges = mst->Edges();

	for(uint32_t i = 0; i < mst->NumEdges(); ++i) {
		if( 1 == edges[i].userSplit ){
			continue;
		}

		if( edges[i].threshold >= stopThreshold ||
		    1 == edges[i].userJoin ){ // join
			if( 1 == edges[i].wasJoined ){
				continue;
			}
			if( JoinInternal(edges[i].node2ID,
							 edges[i].node1ID,
							 edges[i].threshold, i) ){
				edges[i].wasJoined = 1;
			} else {
				edges[i].userSplit = 1;
			}
		} else { // split
			if( 0 == edges[i].wasJoined ){
				continue;
			}
			if( splitChildFromParentInternal(edges[i].node1ID)){
				edges[i].wasJoined = 0;
			} else {
				edges[i].userJoin = 1;
			}
		}
	}

	printf("done\n");
}

bool OmSegmentGraph::JoinInternal( const OmSegID parentID,
								   const OmSegID childUnknownDepthID,
								   const double threshold,
								   const int edgeNumber )
{
	const OmSegID childRootID = graph_getRootID(childUnknownDepthID);
	OmSegment * childRoot = mCache->GetSegmentFromValue(childRootID);
	OmSegment * parent = mCache->GetSegmentFromValue( parentID );

	if( childRoot == mCache->findRoot( parent ) ){
		return false;
	}

	if( childRoot->IsValid() != parent->IsValid() ){
		return false;
	}

	graph_join(childRootID, parentID);

	parent->addChild(childRoot);
	childRoot->setParent(parent, threshold);
	childRoot->setEdgeNumber(edgeNumber);

	mCache->findRoot(parent)->touchFreshnessForMeshes();

	updateSizeListsFromJoin( parent, childRoot );

	return true;
}

bool OmSegmentGraph::splitChildFromParentInternal( const OmSegID childID )
{
	OmSegment * child = mCache->GetSegmentFromValue( childID );

	if( child->getThreshold() > 1 ){
		return false;
	}

	OmSegment* parent = child->getParent();
	if(!parent){ // user manually split?
		return false;
	}

	if( child->IsValid() == parent->IsValid() &&
	    1 == child->IsValid() ){
		return false;
	}

	parent->removeChild(child);
	graph_cut(child->value());
	child->setParent(NULL); // TODO: also set threshold??
	child->setEdgeNumber(-1);

	mCache->findRoot(parent)->touchFreshnessForMeshes();
	child->touchFreshnessForMeshes();

	updateSizeListsFromSplit( parent, child );

	return true;
}

void OmSegmentGraph::updateSizeListsFromJoin(OmSegment* parent, OmSegment* child)
{
	OmSegment * root = mCache->findRoot(parent);
	getSegmentLists()->UpdateFromJoinWorking(root, child);
	getSegmentLists()->UpdateFromJoinValid(root, child); //TODO: we update valid?
}

void OmSegmentGraph::updateSizeListsFromSplit(OmSegment* parent, OmSegment* child)
{
	OmSegment * root = mCache->findRoot(parent);
	uint64_t newChildSize = computeSegmentSizeWithChildren(child->value());
	getSegmentLists()->UpdateFromSplitWorking(root, child, newChildSize);
    //TODO: we don't update valid?
}

quint64 OmSegmentGraph::computeSegmentSizeWithChildren( const OmSegID segID )
{
	quint64 size = 0;
	OmSegmentIteratorLowLevel iter(mCache);
	iter.iterOverSegmentID( segID );
	for(OmSegment * seg = iter.getNextSegment(); NULL != seg; seg = iter.getNextSegment()){
		size += seg->size();
	}
	return size;
}

boost::shared_ptr<OmSegmentLists> OmSegmentGraph::getSegmentLists() {
	return mCache->GetSegmentation()->GetSegmentLists();
}
