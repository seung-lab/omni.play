#ifndef DATA_WRAPPERS
#define DATA_WRAPPERS

#include "system/omSystemTypes.h"
#include "system/omException.h"
#include "volume/omChannel.h"
#include "volume/omVolume.h"

#include <QString>
#include <QHash>

class SegmentDataWrapper;

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
					const OmId mID,
					const QString name,
					const bool enabled );
	OmId getSegmentationID(){ return mSegmentationID; }
	bool isSelected();
	void setSelected( const bool isSelected );
	bool isCheckedOff();
	void setCheckedOff( const bool isCheckedOff );
	QString getNote();
 private:
	OmId mSegmentationID;
	bool mSelected;
	bool mChecked;
};

class FilterDataWrapper : public DataWrapper 
{
 public:
	FilterDataWrapper(){}
	FilterDataWrapper( const OmId mID );
	OmId getChannelID(){ return mChannelID; }
 private:
	OmId mChannelID;
};


class DataWrapperFactory 
{
 public:
	DataWrapperFactory(){}
	DataWrapperFactory( const ObjectType obj_type, const OmId obj_id );
	ObjectType         getType()              { return mType; }

	SegmentationDataWrapper getSegmentationDataWrapper(){ return segmenDW; }
	ChannelDataWrapper getChannelDataWrapper(){ return cDW; }
 private:
	SegmentationDataWrapper segmenDW;
	ChannelDataWrapper cDW;
	ObjectType mType;	
};


#endif
