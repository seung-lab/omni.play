#ifndef OM_SEGMENT_CACHE_IMPL_H
#define OM_SEGMENT_CACHE_IMPL_H

#include "common/omCommon.h"
#include "segment/DynamicTreeContainer.h"
#include "segment/omSegmentPointers.h"
#include "segment/helpers/omSegmentListBySize.h"
#include "segment/helpers/omSegmentListByMRU.h"

#include <QHash>
#include <QSet>
#include <QLinkedList>
#include <map>

// TODO: this was done as proof-of-concept; not sure how much slower 
//  struct constructor is compared to simple int POD... (purcaro)
BOOST_STRONG_TYPEDEF(quint32, PageNum )

class OmMipChunkCoord;
class OmSegment;
class OmSegmentCache;
class OmSegmentEdge;
class OmSegmentation;

class OmSegmentCacheImpl {
 public:
	OmSegmentCacheImpl(OmSegmentation *, OmSegmentCache *);
	~OmSegmentCacheImpl();

	OmSegment* AddSegment();
	void AddSegmentsFromChunk(const OmSegIDsSet &, const OmMipChunkCoord &,
				  boost::unordered_map< OmSegID, unsigned int> * sizes );
	OmSegment* AddSegment(OmSegID value);

	bool isValueAlreadyMappedToSegment( const OmSegID );

	OmSegment* GetSegmentFromValue(const OmSegID value ) {
		if( !mAllPagesLoaded ){
			if ( !isValueAlreadyMappedToSegment( value ) ){
				return NULL;
			}
		}
		return mValueToSegPtrHash[ getValuePageNum(value) ][ value % mPageSize];
	}

	OmSegID GetNumSegments();
	OmSegID GetNumTopSegments();

	bool IsSegmentValid( OmSegID seg);

	bool isSegmentEnabled( OmSegID segID );
	void setSegmentEnabled( OmSegID segID, bool isEnabled );
	void SetAllEnabled(bool);
	OmSegIDsSet& GetEnabledSegmentIdsRef();

	bool isSegmentSelected( OmSegID segID );
	bool isSegmentSelected( OmSegment * seg );
	void setSegmentSelected( OmSegID segID, bool isSelected );
	void SetAllSelected(bool);
	OmSegIDsSet& GetSelectedSegmentIdsRef();
	quint32 numberOfSelectedSegments();
	bool AreSegmentsSelected();
	void UpdateSegmentSelection( const OmSegIDsSet & ids);

	QString getSegmentName( OmSegID segID );
	void setSegmentName( OmSegID segID, QString name );

	QString getSegmentNote( OmSegID segID );
	void setSegmentNote( OmSegID segID, QString note );

	OmSegID getSegmentationID();

	void addToDirtySegmentList( OmSegment* seg);
	void flushDirtySegments();

	OmSegment * findRoot( OmSegment * segment );
	OmSegID findRootID( const OmSegID segID );

	OmSegment * getNMinuxOne(OmSegment * segment);
	OmSegmentEdge * splitTwoChildren(OmSegment * seg1, OmSegment * seg2);
	OmSegmentEdge * splitChildFromParent( OmSegment * child );

	void turnBatchModeOn(const bool batchMode);
	
	OmSegmentEdge * Join( OmSegmentEdge * e );
	void JoinTheseSegments( const OmSegIDsSet & segmentList);
	void UnJoinTheseSegments( const OmSegIDsSet & segmentList);

	quint32 getPageSize() { return mPageSize; }

	void resetGlobalThreshold( const float stopPoint );

	const OmColor & GetColorAtThreshold( OmSegment * segment, const float threshold );

	quint32 getMaxValue(){ return mMaxValue; }

	OmSegPtrListWithPage * getRootLevelSegIDs( const unsigned int offset, const int numToGet, OmSegIDRootType type, OmSegID starSeg = 0);

	void setAsValidated(const OmSegIDsSet & set, const bool valid);
	void buildSegmentSizeLists();

 private:
	bool mAllSelected;
	bool mAllEnabled;

	OmSegID getNextValue();
	OmSegID mMaxValue;

	quint32 mNumSegs;
	quint32 mNumTopLevelSegs;

	OmSegmentation * mSegmentation;

        OmSegIDsSet mEnabledSet;
        OmSegIDsSet mSelectedSet;

	QHash< OmId, QString > segmentCustomNames;
	QHash< OmId, QString > segmentNotes;

	OmSegmentCache * mParentCache;

	bool amInBatchMode;
	bool needToFlush;

	quint32 mPageSize;
	boost::unordered_map< PageNum, std::vector<OmSegment*> > mValueToSegPtrHash;
	QSet< PageNum > validPageNumbers;
	QSet< PageNum > loadedPageNumbers;
	QSet< PageNum > dirtySegmentPages;
	PageNum getValuePageNum( const OmSegID value ){
		return PageNum(value / mPageSize);
	}
	void LoadValuePage( const PageNum valuePageNum );
	void SaveAllLoadedPages();
	void SaveDirtySegmentPages();
	void doSaveSegmentPage( const PageNum segPageNum );
	bool mAllPagesLoaded;

	void clearCaches();
	void invalidateCachedColorFreshness();

	DynamicTreeContainer<OmSegID> * mGraph;
	void initializeDynamicTree();
	void loadDendrogram();
	void loadTreeIfNeeded();
	void doResetGlobalThreshold( const quint32 * dend, 
				     const float * dendValues, 
				     quint8 * edgeDisabledByUser,
				     quint8 * edgeWasJoined,
				     quint8 * edgeForceJoin,
				     const int size, 
				     const float stopPoint );
	bool JoinInternal( const OmSegID parentID, const OmSegID childUnknownDepthID, 
			   const float threshold, const int edgeNumber);
	bool splitChildFromParentInternal( const OmSegID childID );

	QList<OmSegmentEdge*> mManualUserMergeEdgeList;

	OmSegmentEdge * Join( OmSegment *, OmSegment * );
	OmSegmentEdge * Join( const OmId, const OmId );
	void rerootSegmentLists();
	void rerootSegmentList( OmSegIDsSet & set );
	void setSegmentSelectedBatch( OmSegID segID, bool isSelected );

	OmSegmentListBySize mRootListBySize;
	OmSegmentListBySize mValidListBySize;
	OmSegmentListByMRU mRecentRootActivityMap;
	void updateSizeListsFromJoin( OmSegment * root, OmSegment * child );

	void doSelectedSetInsert( const OmSegID segID);
	void doSelectedSetRemove( const OmSegID segID);
	quint64 getRecentActivity();
	void addToRecentMap( const OmSegID segID);
	
	friend class OmSegmentColorizer;
	friend QDataStream &operator<<(QDataStream & out, const OmSegmentCacheImpl & sc );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentCacheImpl & sc );
};

#endif
