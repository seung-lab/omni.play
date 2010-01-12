#include "dataWrappers.h"

DataWrapperFactory::DataWrapperFactory( const ObjectType obj_type, const OmId obj_id )
	: mType( obj_type )
{
	switch( obj_type ){
	case CHANNEL:
		cDW = ChannelDataWrapper( obj_id );
		break;
	case SEGMENTATION:
		segmenDW = SegmentationDataWrapper( obj_id );
		break;
	}
}

ChannelDataWrapper::ChannelDataWrapper( const OmId ID )
	: DataWrapper( ID, CHANNEL )
{
	OmChannel& chann = OmVolume::GetChannel( mID );
	mName = QString::fromStdString( chann.GetName() );
	mEnabled = OmVolume::IsChannelEnabled( mID );
}

QString ChannelDataWrapper::getNote()
{
	const string& str = OmVolume::GetChannel( mID ).GetNote();
	return QString::fromStdString( str );
}


SegmentationDataWrapper::SegmentationDataWrapper( const OmId ID )
	: DataWrapper( ID, SEGMENTATION )
{
	OmSegmentation& segmen = OmVolume::GetSegmentation( mID );
	mName = QString::fromStdString( segmen.GetName() );
	mEnabled = OmVolume::IsSegmentationEnabled( mID );
}

QHash< OmId, FilterDataWrapper > ChannelDataWrapper::getAllFilterIDsAndNames()
{
	QHash< OmId, FilterDataWrapper > filters;

	OmChannel &chann = OmVolume::GetChannel( mID );
	
	foreach( OmId filterID, chann.GetValidFilterIds() ) {
		FilterDataWrapper filter( mID, 
							 filterID,
							 QString::fromStdString( chann.GetFilter(filterID).GetName()),
							 chann.IsFilterEnabled( filterID )
							 );
		filters[ filterID ] = filter;
	}

	return filters;
}

QHash< OmId, SegmentDataWrapper > SegmentationDataWrapper::getAllSegmentIDsAndNames()
{
	QHash< OmId, SegmentDataWrapper > segs;

	OmSegmentation &segmen = OmVolume::GetSegmentation( mID );
	
	foreach( OmId segID, segmen.GetValidSegmentIds() ) {
		SegmentDataWrapper seg( mID, 
						    segID,
						    QString::fromStdString( segmen.GetSegment(segID).GetName()),
						    segmen.IsSegmentEnabled( segID )
						    );
		segs[ segID ] = seg;
	}

	return segs;
}

QString SegmentationDataWrapper::getNote()
{
	const string& str = OmVolume::GetSegmentation(mID).GetNote();
	return QString::fromStdString( str );
}


SegmentDataWrapper::SegmentDataWrapper( const OmId segmentationID, 
								const OmId ID,
								const QString name,
								const bool enabled )
	:  DataWrapper( ID, SEGMENT ), mSegmentationID(segmentationID)
{
	mName     = name;
	mEnabled  = enabled;
	mChecked  = mEnabled; // always initially check-off segment if it is enabled
	mSelected = false;
}


bool SegmentDataWrapper::isSelected()
{
	return mSelected;
}

void SegmentDataWrapper::setSelected( const bool isSelected )
{
	mSelected = isSelected;
}

bool SegmentDataWrapper::isCheckedOff()
{
	return mChecked;
}

void SegmentDataWrapper::setCheckedOff( const bool isCheckedOff )
{
	mChecked = isCheckedOff;
}

QString SegmentDataWrapper::getNote()
{
	const string& str = OmVolume::GetSegmentation(mSegmentationID).GetSegment(mID).GetNote();
	return QString::fromStdString( str );
}


FilterDataWrapper::FilterDataWrapper(  const OmId channelID,
				    const OmId mID,
				    const QString name,
							    const bool enabled )
	: DataWrapper( mID, FILTER ), mChannelID( channelID )
{
	mName = name;
    	mEnabled  = enabled;
}
