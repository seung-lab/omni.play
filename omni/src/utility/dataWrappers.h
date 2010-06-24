#ifndef DATA_WRAPPERS
#define DATA_WRAPPERS

#include "segment/omSegment.h"
#include "common/omException.h"
#include "volume/omChannel.h"
#include "volume/omVolume.h"

#include <QHash>

class SegmentDataWrapper;
class FilterDataWrapper;
class OmSegmentCache;

class ChannelDataWrapper
{
 public:
	ChannelDataWrapper(){}
	ChannelDataWrapper( const OmId mID );
	QHash< OmId, FilterDataWrapper > getAllFilterIDsAndNames();
	QString getNote();
	QString getName();
	bool isEnabled();
	OmId getID()     {    return mID;   }
	ObjectType getType(){ return mType; }
 private:
	OmId mID;
	ObjectType mType;
};

class SegmentationDataWrapper
{
 public:
	SegmentationDataWrapper(){}
	explicit SegmentationDataWrapper( const OmId mID );
	explicit SegmentationDataWrapper( OmSegment * );

	QList < SegmentDataWrapper > getAllSegmentIDsAndNames();
	QString getNote();
	unsigned int getNumberOfSegments();
	unsigned int getNumberOfTopSegments();
	OmSegmentCache * getSegmentCache();
	QString getName();
	bool isEnabled();
	//	QString GetSourceDirectoryPath();
	OmId getID()     {    return mID;   }
	ObjectType getType(){ return mType; }
	quint32 getMaxSegmentValue();
	quint64 getSegmentListSize(OmSegIDRootType type);

 private:
	OmId mID;
	ObjectType mType;
};

class SegmentDataWrapper
{
 public:
	SegmentDataWrapper(){}
	SegmentDataWrapper( const OmId segmentationID, 
			    const OmSegID segmentID );
	SegmentDataWrapper( OmSegment * seg );

	QString getName();

	OmId getSegmentationID(){ return mSegmentationID; }
	QString getSegmentationName();
	
	bool isValid();

	OmSegmentation & getSegmentation();
	OmSegment * getSegment();
	
	bool isSelected();
	void toggleSelected();
	void setSelected( const bool isSelected );
	bool isEnabled();
	void setEnabled(const bool);
	QString getNote();
	void setNote(QString str);
	QString getIDstr();
	OmColor getColorInt();
	Vector3 < float > getColorFloat();
	void setColor(const Vector3 < float >& color);
	void setName( const QString& str );
	OmSegID getID()     {    return mID;   }
	ObjectType getType(){ return mType; }
	quint64 getSize();
	quint64 getSizeWithChildren();
	OmSegmentCache * getSegmentCache();
 private:
	OmSegID mID;
	ObjectType mType;
	OmId mSegmentationID;
};

class FilterDataWrapper
{
 public:
	FilterDataWrapper(){}
	FilterDataWrapper( const OmId channelID,
			   const OmId mID );
	OmId getChannelID(){ return mChannelID; }
	QString getName();
	QString getNote();
	bool isEnabled();
	OmId getID()     {    return mID;   }
	ObjectType getType(){ return mType; }
 private:
	OmId mID;
	ObjectType mType;
	OmId mChannelID;
};

class DataWrapperContainer 
{
 public:
	DataWrapperContainer();
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
