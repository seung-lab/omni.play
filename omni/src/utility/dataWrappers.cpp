#include "common/omDebug.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "utility/dataWrappers.h"
#include "utility/stringHelpers.h"
#include "volume/omSegmentation.h"

/*******************************************
 ****** Data Wrapper Container
 *******************************************/
DataWrapperContainer::DataWrapperContainer()
{
	mIsValidContainer = false;
}

DataWrapperContainer::DataWrapperContainer(const ObjectType obj_type, const OmId obj_id)
{
	mIsValidContainer = false;

	mType = obj_type;
	switch (obj_type) {
	case CHANNEL:
		cDW = ChannelDataWrapper(obj_id);
		mIsValidContainer = true;
		break;
	case SEGMENTATION:
		segmenDW = SegmentationDataWrapper(obj_id);
		mIsValidContainer = true;
		break;
	case VOLUME:
	case SEGMENT:
	case NOTE:
	case FILTER:
		break;
	}
}

DataWrapperContainer::DataWrapperContainer(SegmentationDataWrapper sdw)
{
	mType = SEGMENTATION;
	segmenDW = sdw;
	mIsValidContainer = true;
}

bool DataWrapperContainer::isValidContainer()
{
	return mIsValidContainer;
}

/*******************************************
 ****** Channels
 *******************************************/
ChannelDataWrapper::ChannelDataWrapper(const OmId ID)
	: mID(ID), mType(CHANNEL)
{
}

QString ChannelDataWrapper::getName()
{
	return OmProject::GetChannel(mID).GetName();
}

bool ChannelDataWrapper::isEnabled()
{
	return OmProject::IsChannelEnabled(mID);
}

QString ChannelDataWrapper::getNote()
{
	return OmProject::GetChannel(mID).GetNote();
}

QHash < OmId, FilterDataWrapper > ChannelDataWrapper::getAllFilterIDsAndNames()
{
	QHash < OmId, FilterDataWrapper > filters;

	OmChannel & chann = OmProject::GetChannel(mID);

	foreach(OmId filterID, chann.GetValidFilterIds()) {
		FilterDataWrapper filter(mID, filterID);
		filters[filterID] = filter;
	}

	return filters;
}

/*******************************************
 ****** Segmentations
 *******************************************/
SegmentationDataWrapper::SegmentationDataWrapper(const OmId ID)
	: mID(ID), mType(SEGMENTATION)
{
}

QString SegmentationDataWrapper::getName()
{
	return OmProject::GetSegmentation(mID).GetName();
}

bool SegmentationDataWrapper::isEnabled()
{
	return OmProject::IsSegmentationEnabled(mID);
}

QString SegmentationDataWrapper::getNote()
{
	return OmProject::GetSegmentation(mID).GetNote();
}

unsigned int SegmentationDataWrapper::getNumberOfSegments()
{
	return OmProject::GetSegmentation(mID).GetSegmentCache()->GetNumSegments();
}

unsigned int SegmentationDataWrapper::getNumberOfTopSegments()
{
	return OmProject::GetSegmentation(mID).GetSegmentCache()->GetNumTopSegments();
}

OmSegmentCache * SegmentationDataWrapper::getSegmentCache()
{
	return OmProject::GetSegmentation(mID).GetSegmentCache();
}

quint32 SegmentationDataWrapper::getMaxSegmentValue()
{
	return OmProject::GetSegmentation(mID).GetSegmentCache()->getMaxValue();
}

quint64 SegmentationDataWrapper::getSegmentListSize(OmSegIDRootType type)
{
        return OmProject::GetSegmentation(mID).GetSegmentCache()->getSegmentListSize(type);
}


/*******************************************
 ****** Segments
 *******************************************/
SegmentDataWrapper::SegmentDataWrapper(const OmId segmentationID, const OmSegID segmentID)
	: mID(segmentID)
	, mType(SEGMENT)
	, mSegmentationID( segmentationID )
{
}

bool SegmentDataWrapper::isValid()
{
	return OmProject::IsSegmentationValid(mSegmentationID) &&
		getSegmentCache()->IsSegmentValid(mID);
}

QString SegmentDataWrapper::getName()
{
	return getSegment()->GetName();
}

QString SegmentDataWrapper::getSegmentationName()
{
	return getSegmentation().GetName();
}

bool SegmentDataWrapper::isSelected()
{
	return getSegmentCache()->IsSegmentSelected(mID);
}

void SegmentDataWrapper::setSelected(const bool isSelected)
{
	getSegmentCache()->setSegmentSelected(mID, isSelected);
}

void SegmentDataWrapper::toggleSelected()
{
	getSegmentCache()->setSegmentSelected(mID, !isSelected());
}

bool SegmentDataWrapper::isEnabled()
{
	return getSegmentCache()->isSegmentEnabled(mID);
}

void SegmentDataWrapper::setEnabled(const bool enabled)
{
	getSegmentCache()->setSegmentEnabled(mID, enabled );
}

QString SegmentDataWrapper::getNote()
{
	return getSegment()->GetNote();
}

void SegmentDataWrapper::setNote(QString str)
{
	getSegment()->SetNote(str);
}

QString SegmentDataWrapper::getIDstr()
{
	return QString("%1").arg(getID());
}

OmColor SegmentDataWrapper::getColorInt()
{
	return getSegment()->GetColorInt();
}

Vector3 < float > SegmentDataWrapper::getColorFloat()
{
	return getSegment()->GetColorFloat();
}

void SegmentDataWrapper::setColor(const Vector3 < float >& color)
{
	getSegment()->SetColor( color );
}

void SegmentDataWrapper::setName( const QString& str )
{
	getSegment()->SetName( str );
}

OmSegmentation & SegmentDataWrapper::getSegmentation()
{ 
	return OmProject::GetSegmentation(mSegmentationID); 
}

OmSegment * SegmentDataWrapper::getSegment()
{
	return getSegmentation().GetSegmentCache()->GetSegment( mID );
}

OmSegmentCache * SegmentDataWrapper::getSegmentCache()
{
	return getSegmentation().GetSegmentCache();
}

quint64 SegmentDataWrapper::getSize()
{
	return getSegment()->getSize();
}

quint64 SegmentDataWrapper::getSizeWithChildren()
{
	return getSegment()->getSizeWithChildren();
}

/*******************************************
 ****** Filters
 *******************************************/
FilterDataWrapper::FilterDataWrapper(const OmId channelID, const OmId ID)
	: mID(ID), mType(FILTER), mChannelID(channelID)
{
}

QString FilterDataWrapper::getName()
{
	return OmProject::GetChannel(mChannelID).GetFilter(mID).GetName();
}

bool FilterDataWrapper::isEnabled()
{
	return OmProject::GetChannel(mChannelID).IsFilterEnabled(mID);
}
