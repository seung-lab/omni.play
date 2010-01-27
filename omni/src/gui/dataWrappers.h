#ifndef DATA_WRAPPERS
#define DATA_WRAPPERS

#include "system/omSystemTypes.h"
#include "system/omException.h"
#include "volume/omChannel.h"
#include "volume/omVolume.h"

#include <QString>
#include <QHash>

class SegmentDataWrapper;
class FilterDataWrapper;
class DataWrapper
{
 public:

	DataWrapper(){}
	DataWrapper( const OmId ID, const ObjectType Type )
		: mID(ID), mType(Type) {}

	OmId getID()     {    return mID;   }
     QString getName(){    return mName; }
	ObjectType getType(){ return mType; }
	bool isEnabled()    { return mEnabled; }
 protected:
	OmId mID;
	QString mName;
	ObjectType mType;
	bool mEnabled;
};

class ChannelDataWrapper : public DataWrapper 
{
 public:
	ChannelDataWrapper(){}
	ChannelDataWrapper( const OmId mID );
	QHash< OmId, FilterDataWrapper > getAllFilterIDsAndNames();
	QString getNote();
};

class SegmentationDataWrapper : public DataWrapper 
{
 public:
	SegmentationDataWrapper(){}
	SegmentationDataWrapper( const OmId mID );
	QHash< OmId, SegmentDataWrapper > getAllSegmentIDsAndNames();
	QString getNote();
};

class SegmentDataWrapper : public DataWrapper 
{
 public:
	SegmentDataWrapper(){}
	SegmentDataWrapper( const OmId segmentationID, 
					const OmId segmentID );
	OmId getSegmentationID(){ return mSegmentationID; }
	
	bool isSelected();
	void toggleSelected();
	void setSelected( const bool isSelected );
	bool isEnabled();
	void toggleEnabled();
	QString getNote();
	QString getIDstr();
 private:
	OmId mSegmentationID;
};

class FilterDataWrapper : public DataWrapper 
{
 public:
	FilterDataWrapper(){}
	FilterDataWrapper( const OmId channelID,
				    const OmId mID,
				    const QString name,
				    const bool enabled );
	OmId getChannelID(){ return mChannelID; }
	QString getNote();
 private:
	OmId mChannelID;
};


class DataWrapperContainer 
{
 public:
	DataWrapperContainer(){}
	DataWrapperContainer( const ObjectType obj_type, const OmId obj_id );
	DataWrapperContainer( SegmentationDataWrapper sdw );
	ObjectType getType(){ return mType; }

	SegmentationDataWrapper getSegmentationDataWrapper(){ return segmenDW; }
	ChannelDataWrapper getChannelDataWrapper(){ return cDW; }
 private:
	SegmentationDataWrapper segmenDW;
	ChannelDataWrapper cDW;
	ObjectType mType;	
};


#endif
