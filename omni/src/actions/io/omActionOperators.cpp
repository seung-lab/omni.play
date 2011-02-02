#include "actions/io/omActionOperators.h"
#include "datalayer/archive/omDataArchiveBoost.h"
#include "segment/omSegment.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

#include "actions/details/omSegmentGroupActionImpl.hpp"
#include "actions/details/omSegmentJoinActionImpl.hpp"
#include "actions/details/omSegmentSelectActionImpl.hpp"
#include "actions/details/omSegmentSplitActionImpl.hpp"
#include "actions/details/omSegmentValidateActionImpl.hpp"
#include "actions/details/omSegmentUncertainActionImpl.hpp"
#include "actions/details/omSegmentationThresholdChangeActionImpl.hpp"
#include "actions/details/omVoxelSetValueActionImpl.hpp"
#include "actions/details/omProjectCloseActionImpl.hpp"

QDataStream& operator<<(QDataStream& out, const OmSegmentValidateActionImpl& a)
{
	int version = 1;
	out << version;

	OmSegIDsSet ids;
	std::set<OmSegment*>* segs = a.selectedSegments_.get();
	FOR_EACH(iter, *segs){
		OmSegment* seg = *iter;
		ids.insert(seg->value());
	}
	out << ids;
	out << a.valid_;
	out << a.sdw_.GetSegmentationID();

	return out;
}

QDataStream& operator>>(QDataStream& in, OmSegmentValidateActionImpl& a)
{
	int version;
	in >> version;

	OmSegIDsSet ids;
	in >> ids;
	in >> a.valid_;

	OmID segmentationID;
	in >> segmentationID;
	a.sdw_ = SegmentationDataWrapper(segmentationID);

	SegmentationDataWrapper& sdw = a.sdw_;
	OmSegmentCache* cache = sdw.SegmentCache();
	std::set<OmSegment*>* segs = new std::set<OmSegment*>();
	FOR_EACH(iter, ids){
		OmSegment* seg = cache->GetSegment(*iter);
		segs->insert(seg);
	}

	a.selectedSegments_ = boost::shared_ptr<std::set<OmSegment*> >(segs);

	return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentUncertainActionImpl& a)
{
	int version = 1;
	out << version;

	OmSegIDsSet ids;
	std::set<OmSegment*>* segs = a.selectedSegments_.get();
	FOR_EACH(iter, *segs){
		OmSegment* seg = *iter;
		ids.insert(seg->value());
	}
	out << ids;
	out << a.uncertain_;
	out << a.sdw_.GetSegmentationID();

	return out;
}

QDataStream& operator>>(QDataStream& in, OmSegmentUncertainActionImpl& a)
{
	int version;
	in >> version;

	OmSegIDsSet ids;
	in >> ids;
	in >> a.uncertain_;

	OmID segmentationID;
	in >> segmentationID;
	a.sdw_ = SegmentationDataWrapper(segmentationID);

	SegmentationDataWrapper& sdw = a.sdw_;
	OmSegmentCache* cache = sdw.SegmentCache();
	std::set<OmSegment*>* segs = new std::set<OmSegment*>();
	FOR_EACH(iter, ids){
		OmSegment* seg = cache->GetSegment(*iter);
		segs->insert(seg);
	}

	a.selectedSegments_ = boost::shared_ptr<std::set<OmSegment*> >(segs);

	return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentSplitActionImpl& a)
{
	int version = 2;
	out << version;
	out << a.mEdge;
	out << a.mSegmentationID;
	out << a.desc;
	out << a.mSegID;
	out << a.mCoord1;
	out << a.mCoord2;

	return out;
}

QDataStream& operator>>(QDataStream& in,  OmSegmentSplitActionImpl& a)
{
	int version;
	in >> version;

	in >> a.mEdge;
	in >> a.mSegmentationID;
	in >> a.desc;

	if(1 == version) {
		a.mSegID = 0;
		a.mCoord1 = DataCoord();
		a.mCoord2 = DataCoord();
	} else {
		in >> a.mSegID;
		in >> a.mCoord1;
		in >> a.mCoord2;
	}

	return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentGroupActionImpl& a)
{
	int version = 1;
	out << version;
	out << a.mSegmentationId;
	out << a.mName;
	out << a.mCreate;
	out << a.mSelectedSegmentIds;

	return out;
}

QDataStream& operator>>(QDataStream& in,  OmSegmentGroupActionImpl& a)
{
	int version;
	in >> version;
	in >> a.mSegmentationId;
	in >> a.mName;
	in >> a.mCreate;
	in >> a.mSelectedSegmentIds;

	return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentJoinActionImpl& a)
{
	int version = 1;
	out << version;
	out << a.mSegmentationId;
	out << a.mSelectedSegmentIds;

	return out;
}

QDataStream& operator>>(QDataStream& in,  OmSegmentJoinActionImpl& a)
{
	int version;
	in >> version;
	in >> a.mSegmentationId;
	in >> a.mSelectedSegmentIds;

	return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentSelectActionImpl& a)
{
	int version = 2;
	out << version;
	out << a.sdw_.GetSegmentationID();
	out << a.mNewSelectedIdSet;
	out << a.mOldSelectedIdSet;
	out << a.sdw_.GetSegmentID();
	out << a.augmentListOnly_;

	return out;
}

QDataStream& operator>>(QDataStream& in,  OmSegmentSelectActionImpl& a)
{
	int version;
	in >> version;

	OmID segmentationID;
	in >> segmentationID;

	in >> a.mNewSelectedIdSet;
	in >> a.mOldSelectedIdSet;

	OmSegID segmentID;
	in >> segmentID;

	a.sdw_ = SegmentDataWrapper(segmentationID, segmentID);

	if(version > 1){
		in >> a.augmentListOnly_;
	}

	return in;
}

QDataStream& operator<<(QDataStream& out, const OmProjectSaveActionImpl&)
{
	int version = 1;
	out << version;

	return out;
}

QDataStream& operator>>(QDataStream& in,   OmProjectSaveActionImpl& )
{
	int version;
	in >> version;

	return in;
}

QDataStream& operator<<(QDataStream& out, const OmProjectCloseActionImpl&)
{
	int version = 1;
	out << version;

	return out;
}

QDataStream& operator>>(QDataStream& in,   OmProjectCloseActionImpl& )
{
	int version;
	in >> version;

	return in;
}


QDataStream& operator<<(QDataStream& out, const OmSegmentationThresholdChangeActionImpl& a)
{
	int version = 2;
	out << version;
	out << a.mThreshold;
	out << a.mOldThreshold;
	out << a.mSegmentationId;

	return out;
}

QDataStream& operator>>(QDataStream& in,  OmSegmentationThresholdChangeActionImpl& a)
{
	int version;
	in >> version;
	in >> a.mThreshold;
	in >> a.mOldThreshold;

	if(version > 1){
		in >> a.mSegmentationId;
	} else {
		printf("WARNGING: guessing segmentation ID...\n");
		a.mSegmentationId = 1;
	}

	return in;
}

QDataStream& operator<<(QDataStream& out, const OmVoxelSetValueActionImpl& a)
{
	int version = 1;
	out << version;
	out << a.mSegmentationId;
	//out << a.mOldVoxelValues;	//FIXME
	out << a.mNewValue;

	return out;
}

QDataStream& operator>>(QDataStream& in,  OmVoxelSetValueActionImpl& a)
{
	int version;
	in >> version;
	in >> a.mSegmentationId;
	//in >> a.mOldVoxelValues;	//FIXME
	in >> a.mNewValue;

	return in;
}

