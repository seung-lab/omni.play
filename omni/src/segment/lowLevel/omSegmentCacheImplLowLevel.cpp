#include "segment/lowLevel/omSegmentCacheImplLowLevel.h"
#include "system/omCacheManager.h"
#include "system/omProjectData.h"
#include "volume/omSegmentation.h"

// entry into this class via OmSegmentCache hopefully guarentees proper locking...

OmSegmentCacheImplLowLevel::OmSegmentCacheImplLowLevel( OmSegmentation * segmentation, OmSegmentCache * cache )
	: mSegmentation(segmentation)
	, mParentCache( cache )
	, mSegments( new OmPagingStore<OmSegment>( segmentation, cache ) )
	, mMaxValue(0)
	, mNumSegs(0)
	, mNumTopLevelSegs(0)
	, mAllSelected(false)
	, mAllEnabled(false)
	, mGraph(NULL)
{
}

OmSegmentCacheImplLowLevel::~OmSegmentCacheImplLowLevel()
{
	delete mSegments;
	delete mGraph;
}

bool OmSegmentCacheImplLowLevel::isValueAlreadyMappedToSegment(const OmSegID value)
{
	return mSegments->IsValueAlreadyMapped( value );
}

OmSegment* OmSegmentCacheImplLowLevel::GetSegmentFromValue(const OmSegID value ) 
{
	return mSegments->GetSegmentFromValue( value );
}

OmSegID OmSegmentCacheImplLowLevel::GetNumSegments()
{
	return mNumSegs;
}

OmSegID OmSegmentCacheImplLowLevel::GetNumTopSegments()
{
	return mNumTopLevelSegs;
}

bool OmSegmentCacheImplLowLevel::isSegmentEnabled( OmSegID segID )
{
	const OmSegID rootID = findRoot( GetSegmentFromValue(segID) )->getValue();

	if( mAllEnabled ||
	    mEnabledSet.contains( rootID ) ){
		return true;
	}

	return false;
}

void OmSegmentCacheImplLowLevel::setSegmentEnabled( OmSegID segID, bool isEnabled )
{
	const OmSegID rootID = findRoot( GetSegmentFromValue(segID) )->getValue();
	clearCaches();

	if (isEnabled) {
		mEnabledSet.insert( rootID );
	} else {
		mEnabledSet.remove( rootID );
	}
}

void OmSegmentCacheImplLowLevel::SetAllEnabled(bool enabled)
{
	mAllEnabled = enabled;
	mEnabledSet.clear();
}

OmSegIDsSet & OmSegmentCacheImplLowLevel::GetEnabledSegmentIdsRef()
{
        return mEnabledSet;
}
 
bool OmSegmentCacheImplLowLevel::isSegmentSelected( OmSegment * seg )
{
	const OmSegID rootID = findRoot( seg )->getValue();

	if( mAllSelected ||
	    mSelectedSet.contains( rootID ) ){
		return true;
	}

	return false;
}
 
bool OmSegmentCacheImplLowLevel::isSegmentSelected( OmSegID segID )
{
 	return isSegmentSelected( GetSegmentFromValue( segID ) );
}
 
void OmSegmentCacheImplLowLevel::setSegmentSelected( OmSegID segID, bool isSelected )
{
	setSegmentSelectedBatch( segID, isSelected );
	clearCaches();
} 
 
void OmSegmentCacheImplLowLevel::SetAllSelected(bool selected)
{
        mAllSelected = selected;
 	mSelectedSet.clear();
}
 
OmSegIDsSet & OmSegmentCacheImplLowLevel::GetSelectedSegmentIdsRef()
{
        return mSelectedSet;
} 
 
quint32 OmSegmentCacheImplLowLevel::numberOfSelectedSegments()
{
	return mSelectedSet.size();
}
 
bool OmSegmentCacheImplLowLevel::AreSegmentsSelected()
{
	if( 0 == numberOfSelectedSegments() ){
		return false;
	}

	return true;
}

void OmSegmentCacheImplLowLevel::UpdateSegmentSelection( const OmSegIDsSet & ids )
{
	mSelectedSet.clear();

	OmSegIDsSet::const_iterator iter;
	for( iter = ids.begin(); iter != ids.end(); ++iter ){
		setSegmentSelectedBatch( *iter, true );
	}

	clearCaches();	
}

QString OmSegmentCacheImplLowLevel::getSegmentName( OmSegID segID )
{
	if( segmentCustomNames.contains(segID ) ){
		return segmentCustomNames.value(segID);
	}

	return ""; //QString("segment%1").arg(segID);
}

void OmSegmentCacheImplLowLevel::setSegmentName( OmSegID segID, QString name )
{
	segmentCustomNames[ segID ] = name;
}

QString OmSegmentCacheImplLowLevel::getSegmentNote( OmSegID segID )
{
	if( segmentNotes.contains(segID ) ){
		return segmentNotes.value(segID);
	}

	return "";
}

void OmSegmentCacheImplLowLevel::setSegmentNote( OmSegID segID, QString note )
{
	segmentNotes[ segID ] = note;
}

OmSegID OmSegmentCacheImplLowLevel::getSegmentationID()
{
	return mSegmentation->GetId();
}

void OmSegmentCacheImplLowLevel::addToDirtySegmentList( OmSegment* seg)
{
	mSegments->AddToDirtyList( seg->mValue );
}

void OmSegmentCacheImplLowLevel::flushDirtySegments()
{
	mSegments->FlushDirtyItems();
}

OmSegment * OmSegmentCacheImplLowLevel::findRoot( OmSegment * segment )
{
	if(0 == segment->mParentSegID) {
		return segment;
	}

	DynamicTree<OmSegID> * rootDT  = mGraph->get( segment->mValue )->findRoot();
	
	return GetSegmentFromValue( rootDT->getKey() );
}

OmSegID OmSegmentCacheImplLowLevel::findRootID( const OmSegID segID )
{
	return findRoot( GetSegmentFromValue( segID ) )->getValue();
}

void OmSegmentCacheImplLowLevel::turnBatchModeOn( const bool batchMode )
{
	mSegments->SetBatchMode(batchMode);
}

quint32 OmSegmentCacheImplLowLevel::getPageSize() 
{ 
	return mSegments->getPageSize(); 
}

quint32 OmSegmentCacheImplLowLevel::getMaxValue()
{ 
	return mMaxValue; 
}

///////////////////////////
////// Private 
///////////////////////////

OmSegID OmSegmentCacheImplLowLevel::getNextValue()
{
	++mMaxValue;
	return mMaxValue;
}

void OmSegmentCacheImplLowLevel::clearCaches()
{
	OmCacheManager::Freshen(true);
}

void OmSegmentCacheImplLowLevel::setSegmentSelectedBatch( OmSegID segID, bool isSelected )
{
       const OmSegID rootID = findRoot( GetSegmentFromValue(segID) )->getValue();

       if (isSelected) {
               doSelectedSetInsert( rootID );
       } else {
               doSelectedSetRemove( rootID );
               assert( !mSelectedSet.contains( segID ));
       }
}

void OmSegmentCacheImplLowLevel::updateSizeListsFromJoin( OmSegment * parent, OmSegment * child )
{
	OmSegment * root = findRoot(parent);
	mRootListBySize.updateFromJoin( root, child );
	mValidListBySize.updateFromJoin( root, child );
}

void OmSegmentCacheImplLowLevel::doSelectedSetInsert( const OmSegID segID)
{
	mSelectedSet.insert( segID );
	addToRecentMap(segID);
}
 		
void OmSegmentCacheImplLowLevel::doSelectedSetRemove( const OmSegID segID)
{
	mSelectedSet.remove( segID );
	addToRecentMap(segID);
}

quint64 OmSegmentCacheImplLowLevel::getRecentActivity()
{
	static quint64 activity = 0;
	++activity;
	return activity;
}
	
void OmSegmentCacheImplLowLevel::addToRecentMap( const OmSegID segID )
{
	mRecentRootActivityMap.touch( segID, getRecentActivity() );
}

///////////////////////////
////// Tree-stuff

void OmSegmentCacheImplLowLevel::initializeDynamicTree()
{
	delete mGraph;

	// mMaxValue is a valid segment id, so array needs to be 1 bigger
	const int size =  mMaxValue + 1;
	
	mGraph = new DynamicTreeContainer<OmSegID>( size );
}

void OmSegmentCacheImplLowLevel::rerootSegmentLists()
{
	rerootSegmentList( mEnabledSet );
	rerootSegmentList( mSelectedSet );
}

void OmSegmentCacheImplLowLevel::rerootSegmentList( OmSegIDsSet & set )
{
	OmSegIDsSet old = set;
	set.clear();

	OmSegID rootSegID;
	foreach( const OmSegID & id, old ){
		rootSegID = findRoot( GetSegmentFromValue( id) )->getValue();
		set.insert( rootSegID );
	}
}

void OmSegmentCacheImplLowLevel::buildSegmentSizeLists()
{
	OmSegment * seg;
	for( quint32 i = 0; i <= mMaxValue; ++i ){
		seg = GetSegmentFromValue( i );
                if( NULL == seg) {
			continue;
		} 
		mRootListBySize.insertSegment( seg );
	}
}

void OmSegmentCacheImplLowLevel::resetGlobalThreshold( const float stopPoint )
{
	printf("setting global threshold to %f...\n", stopPoint);

	doResetGlobalThreshold( mSegmentation->mDend->getQuint32Ptr(), 
				mSegmentation->mDendValues->getFloatPtr(), 
				mSegmentation->mEdgeDisabledByUser->getQuint8Ptr(), 
				mSegmentation->mEdgeWasJoined->getQuint8Ptr(), 
				mSegmentation->mEdgeForceJoin->getQuint8Ptr(), 
				mSegmentation->mDendCount, 
				stopPoint);

	mSelectedSet.clear(); // nuke selected set for now...
	//rerootSegmentLists();
	clearCaches();


	printf("done\n");
}

// TODO: store more threshold info in the segment cache, and reduce size of walk...
// NOTE: assuming incoming data is an edge list
void OmSegmentCacheImplLowLevel::doResetGlobalThreshold( const quint32 * nodes, 
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

bool OmSegmentCacheImplLowLevel::JoinInternal( const OmSegID parentID, 
				       const OmSegID childUnknownDepthID, 
				       const float threshold,
				       const int edgeNumber )
{
	DynamicTree<OmSegID> * childRootDT = mGraph->get( childUnknownDepthID )->findRoot();

	OmSegment * childRoot = GetSegmentFromValue( childRootDT->getKey() );
	OmSegment * parent = GetSegmentFromValue( parentID );

	if( childRoot == findRoot( parent ) ){
		return false;
	}
	
	if( childRoot->mImmutable != parent->mImmutable ){
		printf("not joining child %d to parent %d: child immutability is %d, but parent's is %d\n",
		       childRoot->mValue, parent->mValue, childRoot->mImmutable, parent->mImmutable );
		return false;
	}
 
	childRootDT->join( mGraph->get( parentID ) );

	parent->segmentsJoinedIntoMe.insert( childRoot->mValue );
	childRoot->setParent(parent, threshold);
	childRoot->mEdgeNumber = edgeNumber;

	updateSizeListsFromJoin( findRoot(parent), childRoot );

	--mNumTopLevelSegs;

	return true;
}

bool OmSegmentCacheImplLowLevel::splitChildFromParentInternal( const OmSegID childID )
{
	OmSegment * child = GetSegmentFromValue( childID );

	if( child->mThreshold > 1 ){
		return false;
	}

	assert( child->mParentSegID );

	OmSegment * parent = GetSegmentFromValue( child->mParentSegID );

	if( child->mImmutable == parent->mImmutable &&
	    1 == child->mImmutable ){
		printf("not splitting child %d from parent %d: child immutability is %d and parent's is %d\n",
		       child->mValue, parent->mValue, child->mImmutable, parent->mImmutable );
		return false;
	}
	
	parent->segmentsJoinedIntoMe.erase( child->mValue );
        mGraph->get( child->mValue )->cut();
	child->mParentSegID = 0;
	child->mEdgeNumber = -1;

	++mNumTopLevelSegs;

	return true;
}
