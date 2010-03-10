#include "dataWrappers.h"
#include "common/omDebug.h"

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
	mName = QString::fromStdString(chann.GetName());
	mEnabled = OmVolume::IsChannelEnabled(mID);
}

QString ChannelDataWrapper::getNote()
{
	const string & str = OmVolume::GetChannel(mID).GetNote();
	return QString::fromStdString(str);
}

QHash < OmId, FilterDataWrapper > ChannelDataWrapper::getAllFilterIDsAndNames()
{
	QHash < OmId, FilterDataWrapper > filters;

	OmChannel & chann = OmVolume::GetChannel(mID);

	foreach(OmId filterID, chann.GetValidFilterIds()) {
		FilterDataWrapper filter(mID,
					 filterID,
					 QString::fromStdString(chann.GetFilter(filterID).GetName()),
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
	mName = QString::fromStdString(segmen.GetName());
	mEnabled = OmVolume::IsSegmentationEnabled(mID);
}

QList < SegmentDataWrapper > SegmentationDataWrapper::getAllSegmentIDsAndNames()
{
	QList < SegmentDataWrapper > segs;

	OmSegmentation & segmen = OmVolume::GetSegmentation(mID);

	foreach(OmId segID, segmen.GetValidSegmentIds()) {
		SegmentDataWrapper seg(mID, segID);
		segs << seg;
	}

	return segs;
}

QString SegmentationDataWrapper::getNote()
{
	const string & str = OmVolume::GetSegmentation(mID).GetNote();
	return QString::fromStdString(str);
}

unsigned int SegmentationDataWrapper::getNumberOfSegments()
{
	return OmVolume::GetSegmentation(mID).GetValidSegmentIds().size();
}

QString SegmentationDataWrapper::GetSourceDirectoryPath()
{
	return QString::fromStdString( OmVolume::GetSegmentation(mID).GetSourceDirectoryPath() );
}

QString SegmentationDataWrapper::GetSourceFilenameRegex()
{
	return QString::fromStdString( OmVolume::GetSegmentation(mID).GetSourceFilenameRegex() );
}

const list < string > SegmentationDataWrapper::GetSourceFilenameRegexMatches()
{
	return OmVolume::GetSegmentation(mID).GetSourceFilenameRegexMatches();
}


/*******************************************
 ****** Segments
 *******************************************/
SegmentDataWrapper::SegmentDataWrapper(const OmId segmentationID, const OmId segmentID)
 : DataWrapper(segmentID, SEGMENT)
{
	mSegmentationID = segmentationID;
	mName = QString::fromStdString(OmVolume::GetSegmentation(mSegmentationID).GetSegment(segmentID).GetName());
}

QString SegmentDataWrapper::getSegmentationName()
{
	return QString::fromStdString(OmVolume::GetSegmentation(mSegmentationID).GetName());
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
	const string & str = OmVolume::GetSegmentation(mSegmentationID).GetSegment(mID).GetNote();
	return QString::fromStdString(str);
}

void SegmentDataWrapper::setNote(QString str)
{
	OmVolume::GetSegmentation(mSegmentationID).GetSegment(mID).SetNote(qPrintable(str));
}

QString SegmentDataWrapper::getIDstr()
{
	return QString("%1").arg(getID());
}

const Vector3 < float >& SegmentDataWrapper::getColor()
{
	return OmVolume::GetSegmentation(mSegmentationID).GetSegment(mID).GetColor();
}

void SegmentDataWrapper::setColor(const Vector3 < float >& color)
{
	OmVolume::GetSegmentation(mSegmentationID).GetSegment(mID).SetColor( color );
}

void SegmentDataWrapper::setName( const QString& str )
{
	OmVolume::GetSegmentation(mSegmentationID).GetSegment(mID).SetName( str.toStdString() );
}

QString SegmentDataWrapper::getDataValuesForSegment()
{
	const SegmentDataSet & data_set = OmVolume::GetSegmentation(mSegmentationID).GetValuesMappedToSegmentId(mID);
	
	if( data_set.size() == 0 ){
		return "";
	}

	QString str;
	unsigned int counter = 0;
	OmIds::iterator itr;
	for( itr = data_set.begin(); itr != data_set.end(); itr++ ){
		counter++;
		str += QString::number( *itr );
		if( counter < data_set.size() ){
			str += ", ";
			if( counter > 0 && counter % 8 == 0 ){
				str += "\n";
			}
		}
	}
	return str;
}

QString SegmentDataWrapper::get_original_mapped_data_value()
{
	
	const SEGMENT_DATA_TYPE value = OmVolume::GetSegmentation(mSegmentationID).GetSegment(mID).get_original_mapped_data_value();
	if( 0 == value ){
		return "<not set>";
	} else {
		return QString::number( value );
	}
	
	//	return QString::number( OmVolume::GetSegmentation(mSegmentationID).GetSegment(mID).get_original_mapped_data_value() );
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
