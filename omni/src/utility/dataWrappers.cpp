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
	OmChannel & chann = OmVolume::GetChannel(mID);
	mName = chann.GetName();
	mEnabled = OmVolume::IsChannelEnabled(mID);
}

QString ChannelDataWrapper::getNote()
{
	return OmVolume::GetChannel(mID).GetNote();
}

QHash < OmId, FilterDataWrapper > ChannelDataWrapper::getAllFilterIDsAndNames()
{
	QHash < OmId, FilterDataWrapper > filters;

	OmChannel & chann = OmVolume::GetChannel(mID);

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
	OmSegmentation & segmen = OmVolume::GetSegmentation(mID);
	mName = segmen.GetName();
	mEnabled = OmVolume::IsSegmentationEnabled(mID);
}

QString SegmentationDataWrapper::getNote()
{
	return OmVolume::GetSegmentation(mID).GetNote();
}

unsigned int SegmentationDataWrapper::getNumberOfSegments()
{
	return OmVolume::GetSegmentation(mID).GetNumSegments();
}

unsigned int SegmentationDataWrapper::getNumberOfTopSegments()
{
	return OmVolume::GetSegmentation(mID).GetNumTopSegments();
}

/*******************************************
 ****** Segments
 *******************************************/
SegmentDataWrapper::SegmentDataWrapper(const OmId segmentationID, const OmId segmentID)
 : DataWrapper(segmentID, SEGMENT)
{
	mSegmentationID = segmentationID;
	mName = OmVolume::GetSegmentation(mSegmentationID).GetSegment(segmentID)->GetName();
}

QString SegmentDataWrapper::getSegmentationName()
{
	return OmVolume::GetSegmentation(mSegmentationID).GetName();
}

bool SegmentDataWrapper::isSelected()
{
	return OmVolume::GetSegmentation(mSegmentationID).IsSegmentSelected(mID);
}

void SegmentDataWrapper::setSelected(const bool isSelected)
{
	OmVolume::GetSegmentation(mSegmentationID).SetSegmentSelected(mID, isSelected);
}

void SegmentDataWrapper::toggleSelected()
{
	OmVolume::GetSegmentation(mSegmentationID).SetSegmentSelected(mID, !isSelected());
}

bool SegmentDataWrapper::isEnabled()
{
	return OmVolume::GetSegmentation(mSegmentationID).IsSegmentEnabled(mID);
}

void SegmentDataWrapper::toggleEnabled()
{
	OmVolume::GetSegmentation(mSegmentationID).SetSegmentEnabled(mID, !isEnabled());
}

void SegmentDataWrapper::setEnabled(const bool enabled)
{
	OmVolume::GetSegmentation(mSegmentationID).SetSegmentEnabled(mID, enabled );
}

QString SegmentDataWrapper::getNote()
{
	return OmVolume::GetSegmentation(mSegmentationID).GetSegment(mID)->GetNote();
}

void SegmentDataWrapper::setNote(QString str)
{
	OmVolume::GetSegmentation(mSegmentationID).GetSegment(mID)->SetNote(str);
}

QString SegmentDataWrapper::getIDstr()
{
	return QString("%1").arg(getID());
}

const Vector3 < float >& SegmentDataWrapper::getColor()
{
	return OmVolume::GetSegmentation(mSegmentationID).GetSegment(mID)->GetColor();
}

void SegmentDataWrapper::setColor(const Vector3 < float >& color)
{
	OmVolume::GetSegmentation(mSegmentationID).GetSegment(mID)->SetColor( color );
}

void SegmentDataWrapper::setName( const QString& str )
{
	OmVolume::GetSegmentation(mSegmentationID).GetSegment(mID)->SetName( str );
}

QString SegmentDataWrapper::getDataValuesForSegment()
{
	const SegmentDataSet & data_set = OmVolume::GetSegmentation(mSegmentationID).GetValuesMappedToSegmentId(mID);
	
	return StringHelpers::getStringFromSegmentSet( data_set );
}

QString SegmentDataWrapper::get_original_mapped_data_value()
{
	const SEGMENT_DATA_TYPE value = OmVolume::GetSegmentation(mSegmentationID).GetSegment(mID)->getValue();
	if( 0 == value ){
		return "<not set>";
	} else {
		return QString::number( value );
	}
}

QString SegmentDataWrapper::chunkListStr()
{
	QStringList strs;
	foreach( OmMipChunkCoord c, OmVolume::GetSegmentation(mSegmentationID).GetSegment(mID)->getChunks() ){
		strs << QString("[%1:%2,%3,%4]")
			.arg(c.getLevel())
			.arg(c.getCoordinateX())
			.arg(c.getCoordinateY())
			.arg(c.getCoordinateZ());
	}
	
	return StringHelpers::getStringFromStringList( strs );
}

OmSegmentation & SegmentDataWrapper::getSegmentation()
{ 
	return OmVolume::GetSegmentation(mSegmentationID); 
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
