#include "project/omProject.h"
#include "dataWrappers.h"
#include "common/omDebug.h"
#include "utility/stringHelpers.h"
#include "segment/omSegmentCache.h"

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
	return OmProject::GetSegmentation(mID).GetNumSegments();
}

unsigned int SegmentationDataWrapper::getNumberOfTopSegments()
{
	return OmProject::GetSegmentation(mID).GetNumTopSegments();
}

OmSegmentCache * SegmentationDataWrapper::getSegmentCache()
{
	return OmProject::GetSegmentation(mID).GetSegmentCache();
}


/*******************************************
 ****** Segments
 *******************************************/
SegmentDataWrapper::SegmentDataWrapper(const OmId segmentationID, const OmId segmentID)
	: mID(segmentID), mType(SEGMENT), 
	  mSegmentationID( segmentationID )
{
}

QString SegmentDataWrapper::getName()
{
	return OmProject::GetSegmentation(mSegmentationID).GetSegment(mID)->GetName();
}

QString SegmentDataWrapper::getSegmentationName()
{
	return OmProject::GetSegmentation(mSegmentationID).GetName();
}

bool SegmentDataWrapper::isSelected()
{
	return OmProject::GetSegmentation(mSegmentationID).IsSegmentSelected(mID);
}

void SegmentDataWrapper::setSelected(const bool isSelected)
{
	OmProject::GetSegmentation(mSegmentationID).SetSegmentSelected(mID, isSelected);
}

void SegmentDataWrapper::toggleSelected()
{
	OmProject::GetSegmentation(mSegmentationID).SetSegmentSelected(mID, !isSelected());
}

bool SegmentDataWrapper::isEnabled()
{
	return OmProject::GetSegmentation(mSegmentationID).IsSegmentEnabled(mID);
}

void SegmentDataWrapper::toggleEnabled()
{
	OmProject::GetSegmentation(mSegmentationID).SetSegmentEnabled(mID, !isEnabled());
}

void SegmentDataWrapper::setEnabled(const bool enabled)
{
	OmProject::GetSegmentation(mSegmentationID).SetSegmentEnabled(mID, enabled );
}

QString SegmentDataWrapper::getNote()
{
	return OmProject::GetSegmentation(mSegmentationID).GetSegment(mID)->GetNote();
}

void SegmentDataWrapper::setNote(QString str)
{
	OmProject::GetSegmentation(mSegmentationID).GetSegment(mID)->SetNote(str);
}

QString SegmentDataWrapper::getIDstr()
{
	return QString("%1").arg(getID());
}

Vector3 < float > SegmentDataWrapper::getColor()
{
	return OmProject::GetSegmentation(mSegmentationID).GetSegment(mID)->GetColorFloat();
}

void SegmentDataWrapper::setColor(const Vector3 < float >& color)
{
	OmProject::GetSegmentation(mSegmentationID).GetSegment(mID)->SetColor( color );
}

void SegmentDataWrapper::setName( const QString& str )
{
	OmProject::GetSegmentation(mSegmentationID).GetSegment(mID)->SetName( str );
}

QString SegmentDataWrapper::get_original_mapped_data_value()
{
	const SEGMENT_DATA_TYPE value = OmProject::GetSegmentation(mSegmentationID).GetSegment(mID)->getValue();
	if( 0 == value ){
		return "<not set>";
	} else {
		return QString::number( value );
	}
}

OmSegmentation & SegmentDataWrapper::getSegmentation()
{ 
	return OmProject::GetSegmentation(mSegmentationID); 
}

OmSegment * SegmentDataWrapper::getSegment()
{
	return getSegmentation().GetSegment( mID );
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
