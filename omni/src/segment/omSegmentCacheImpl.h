#ifndef OM_SEGMENT_CACHE_IMPL_H
#define OM_SEGMENT_CACHE_IMPL_H

#include "volume/omSegmentation.h"
#include "utility/DynamicTreeContainer.h"
#include <boost/tr1/unordered_map.hpp>

 //TODO: this was done as proof-of-concept; not sure how much slower struct is compared to simple int POD... (purcaro)
#include "boost/strong_typedef.hpp"
BOOST_STRONG_TYPEDEF(quint32, PageNum )

class OmSegPtrList {
 public:
	OmSegPtrList()
		: isValid(false) {}
	OmSegPtrList( const std::vector<OmSegment*> & L )
		: isValid(true), list(L) {}
	
	bool isValid;
	std::vector<OmSegment*> list;
};

class OmSegmentCacheImpl {
public:
	OmSegmentCacheImpl(OmSegmentation * segmentation, OmSegmentCache * cache);
	~OmSegmentCacheImpl();

	OmSegment* AddSegment();
	void AddSegmentsFromChunk(const SegmentDataSet & values, const OmMipChunkCoord & mipCoord);
	OmSegment* AddSegment(OmSegID value);

	bool isValueAlreadyMappedToSegment( const OmSegID & );

	OmSegment* GetSegmentFromValue( const OmSegID & );

	OmId GetNumSegments();
	OmId GetNumTopSegments();

	bool IsSegmentValid(OmId seg);

	bool isSegmentEnabled( OmId segID );
	void setSegmentEnabled( OmId segID, bool isEnabled );
	void SetAllEnabled(bool);
	OmIds& GetEnabledSegmentIdsRef();

	bool isSegmentSelected( OmId segID );
	bool isSegmentSelected( OmSegment * seg );
	void setSegmentSelected( OmId segID, bool isSelected );
	void SetAllSelected(bool);
	OmIds& GetSelectedSegmentIdsRef();
	quint32 numberOfSelectedSegments();
	bool AreSegmentsSelected();

	QString getSegmentName( OmId segID );
	void setSegmentName( OmId segID, QString name );

	QString getSegmentNote( OmId segID );
	void setSegmentNote( OmId segID, QString note );

	OmId getSegmentationID();

	void addToDirtySegmentList( OmSegment* seg);
	void flushDirtySegments();

	OmSegment * findRoot( OmSegment * segment );

	OmSegment * getNMinuxOne(OmSegment * segment);
	void splitChildLowestThreshold( OmSegment * segment );
	void splitTwoChildren(OmSegment * seg1, OmSegment * seg2);
	void splitChildFromParent( OmSegment * child );

	void turnBatchModeOn(const bool batchMode);
	
	void JoinAllSegmentsInSelectedList();

	quint32 getPageSize() { return mPageSize; }

	void setSegmentListDirectCache( const OmMipChunkCoord & chunkCoord,
					std::vector< OmSegment* > & segmentsToDraw );
	bool segmentListDirectCacheHasCoord( const OmMipChunkCoord & chunkCoord );
	std::vector<OmSegment*> & getSegmentListDirectCache( const OmMipChunkCoord & chunkCoord );

	void reloadDendrogram( const quint32 * dend, const float * dendValues, 
			       const int size, const float stopPoint );

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

        OmIds mEnabledSet;
        OmIds mSelectedSet;
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
	void SaveAllPages();
	void SaveDirtySegmentPages();
	void doSaveSegmentPage( const PageNum segPageNum );
	bool mAllPagesLoaded;

	OmSegment* GetSegmentFromValueFast(const OmSegID & value) {
		if( !mAllPagesLoaded ){
			if ( !isValueAlreadyMappedToSegment( value ) ){
				return NULL;
			}
		}
		return mValueToSegPtrHash[ getValuePageNum(value) ][ value % mPageSize];
	}

	boost::unordered_map< int,
		boost::unordered_map< int,
		boost::unordered_map< int,
		boost::unordered_map< int, OmSegPtrList > > > > cacheDirectSegmentList;
	void clearCaches();
	void invalidateCachedColorFreshness();
	quint32 mCachedColorFreshness;

	DynamicTreeContainer<OmSegID> * mTree;
	void initializeDynamicTree();
	void loadDendrogram( const quint32 * dend, const float * dendValues, 
			     const int size, const float stopPoint );
	void doLoadDendrogram();
	void loadTreeIfNeeded();

	void Join(OmSegment * parent, OmSegment * childUnknownLevel, float threshold);
	void Join( const OmId, const OmId, const float );
	void clearAllJoins();
	void rerootSegmentLists();
	void rerootSegmentList( OmIds & set );

	friend class OmSegmentColorizer;

	friend QDataStream &operator<<(QDataStream & out, const OmSegmentCacheImpl & sc );
	friend QDataStream &operator>>(QDataStream & in, OmSegmentCacheImpl & sc );
};

#endif
