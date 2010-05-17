#include "project/omProject.h"
#include "dataWrappers.h"
#include "common/omDebug.h"
#include "utility/stringHelpers.h"

/*******************************************
 ****** Data Wrapper Container
 *******************************************/
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
:DataWrapper(ID, CHANNEL)
{
	OmChannel & chann = OmProject::GetChannel(mID);
	mName = chann.GetName();
	mEnabled = OmProject::IsChannelEnabled(mID);
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
		FilterDataWrapper filter(mID,
					 filterID,
					 chann.GetFilter(filterID).GetName(),
					 chann.IsFilterEnabled(filterID)
		    );
		filters[filterID] = filter;
	}

	return filters;
}

/*******************************************
 ****** Segmentations
 *******************************************/
SegmentationDataWrapper::SegmentationDataWrapper(const OmId ID)
 : DataWrapper(ID, SEGMENTATION)
{
	OmSegmentation & segmen = OmProject::GetSegmentation(mID);
	mName = segmen.GetName();
	mEnabled = OmProject::IsSegmentationEnabled(mID);
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

/*******************************************
 ****** Segments
 *******************************************/
SegmentDataWrapper::SegmentDataWrapper(const OmId segmentationID, const OmId segmentID)
 : DataWrapper(segmentID, SEGMENT)
{
	mSegmentationID = segmentationID;
	mName = OmProject::GetSegmentation(mSegmentationID).GetSegment(segmentID)->GetName();
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

const Vector3 < float >& SegmentDataWrapper::getColor()
{
	return OmProject::GetSegmentation(mSegmentationID).GetSegment(mID)->GetColor();
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
FilterDataWrapper::FilterDataWrapper(const OmId channelID, const OmId mID, const QString name, const bool enabled)
 : DataWrapper(mID, FILTER), mChannelID(channelID)
{
	mName = name;
	mEnabled = enabled;
}
