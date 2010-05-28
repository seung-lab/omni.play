#ifndef OM_SEGMENT_CACHE_IMPL_H
#define OM_SEGMENT_CACHE_IMPL_H

#include "common/omCommon.h"
#include "segment/DynamicTreeContainer.h"
#include "segment/omSegmentPointers.h"

#include <QSet>
#include <boost/dynamic_bitset.hpp>

// TODO: this was done as proof-of-concept; not sure how much slower 
//  struct constructor is compared to simple int POD... (purcaro)
BOOST_STRONG_TYPEDEF(quint32, PageNum )

class OmSegment;
class OmSegmentCache;
class OmSegmentation;
class OmMipChunkCoord;

class OmSegmentCacheImpl {
 public:
	OmSegmentCacheImpl(OmSegmentation *, OmSegmentCache *);
	~OmSegmentCacheImpl();

	OmSegment* AddSegment();
	void AddSegmentsFromChunk(const OmSegIDs &, const OmMipChunkCoord &);
	OmSegment* AddSegment(OmSegID value);

	bool isValueAlreadyMappedToSegment( const OmSegID & );

	OmSegment* GetSegmentFromValue(const OmSegID & value) {
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
	OmSegIDs& GetEnabledSegmentIdsRef();

	bool isSegmentSelected( OmSegID segID );
	bool isSegmentSelected( OmSegment * seg );
	void setSegmentSelected( OmSegID segID, bool isSelected );
	void SetAllSelected(bool);
	OmSegIDs& GetSelectedSegmentIdsRef();
	quint32 numberOfSelectedSegments();
	bool AreSegmentsSelected();
	void UpdateSegmentSelection( const OmSegIDs & ids, const bool areSelected );

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
	void splitChildLowestThreshold( OmSegment * segment );
	void splitTwoChildren(OmSegment * seg1, OmSegment * seg2);
	void splitChildFromParent( OmSegment * child );

	void turnBatchModeOn(const bool batchMode);
	
	void JoinAllSegmentsInSelectedList();
	void JoinTheseSegments(OmIds segmentList);
	void UnJoinTheseSegments(OmIds segmentList);

	quint32 getPageSize() { return mPageSize; }

	void setSegmentListDirectCache( const OmMipChunkCoord & chunkCoord,
					std::vector< OmSegment* > & segmentsToDraw );
	bool segmentListDirectCacheHasCoord( const OmMipChunkCoord & chunkCoord );
	const OmSegPtrs & getSegmentListDirectCache( const OmMipChunkCoord & chunkCoord );

	void resetGlobalThreshold( const float stopPoint );

	const OmColor & GetColorAtThreshold( OmSegment * segment, const float threshold );

	quint32 getMaxValue(){ return mMaxValue; }

 private:
	bool mAllSelected;
	bool mAllEnabled;

	OmSegID getNextValue();
	OmSegID mMaxValue;

	quint32 mNumSegs;
	quint32 mNumTopLevelSegs;

	OmSegmentation * mSegmentation;

        OmSegIDs mEnabledSet;
        OmSegIDs mSelectedSet;
	QHash< OmId, QString > segmentCustomNames;
	QHash< OmId, QString > segmentNotes;

	OmSegmentCache * mParentCache;

	bool amInBatchMode;
	bool needToFlush;

	quint32 mPageSize;
	boost::unordered_map< PageNum, OmSegment** > mValueToSegPtrHash;
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

	boost::unordered_map< int, 
		boost::unordered_map< int,
		boost::unordered_map< int,
		boost::unordered_map< int, OmSegPtrsValid > > > > cacheDirectSegmentList;
	void clearCaches();
	void invalidateCachedColorFreshness();
	quint32 mCachedColorFreshness;

	DynamicTreeContainer<OmSegID> * mGraph;
	void initializeDynamicTree();
	void doLoadDendrogram( const quint32 *, const float *, 
			       const int, const float );
	void loadDendrogram();
	void loadTreeIfNeeded();

	void Join( OmSegment *, OmSegment *, const float );
	void Join( const OmId, const OmId, const float );
	void rerootSegmentLists();
	void rerootSegmentList( OmSegIDs & set );

	boost::dynamic_bitset<> rootSegs;

	friend class OmSegmentColorizer;
	friend QDataStream &operator<<(QDataStream & out, const OmSegmentCacheImpl & sc );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentCacheImpl & sc );
};

#endif
