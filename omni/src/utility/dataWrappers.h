#ifndef DATA_WRAPPERS
#define DATA_WRAPPERS

#include "segment/omSegment.h"
#include "common/omException.h"
#include "volume/omChannel.h"
#include "volume/omVolume.h"

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
	QList < SegmentDataWrapper > getAllSegmentIDsAndNames();
	QString getNote();
	unsigned int getNumberOfSegments();
	unsigned int getNumberOfTopSegments();
	//	QString GetSourceDirectoryPath();
};

class SegmentDataWrapper : public DataWrapper 
{
 public:
	SegmentDataWrapper(){}
	SegmentDataWrapper( const OmId segmentationID, 
			    const OmId segmentID );
	OmId getSegmentationID(){ return mSegmentationID; }
	QString getSegmentationName();
	
	OmSegmentation & getSegmentation();
	OmSegment * getSegment();
	
	bool isSelected();
	void toggleSelected();
	void setSelected( const bool isSelected );
	bool isEnabled();
	void setEnabled(const bool);
	void toggleEnabled();
	QString getNote();
	void setNote(QString str);
	QString getIDstr();
	const Vector3 < float >& getColor();
	void setColor(const Vector3 < float >& color);
	void setName( const QString& str );
	QString getDataValuesForSegment();
	QString get_original_mapped_data_value();
	QString chunkListStr();
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

	bool isValidContainer();
 private:
	SegmentationDataWrapper segmenDW;
	ChannelDataWrapper cDW;
	ObjectType mType;
	bool mIsValidContainer;
};


#endif
