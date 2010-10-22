#include "utility/dataWrappers.h"
#include "datalayer/fs/omActionLoggerFS.h"
#include "datalayer/archive/omDataArchiveBoost.h"
#include "volume/omSegmentation.h"
#include "segment/omSegment.h"

#include "actions/omSegmentGroupAction.h"
#include "actions/omSegmentJoinAction.h"
#include "actions/omSegmentSelectAction.h"
#include "actions/omSegmentSplitAction.h"
#include "actions/omSegmentValidateAction.h"
#include "actions/omSegmentUncertainAction.h"
#include "actions/omSegmentationThresholdChangeAction.h"
#include "actions/omVoxelSetValueAction.h"

OmActionLoggerFS::OmActionLoggerFS()
	: initialized(false)
{}

OmActionLoggerFS::~OmActionLoggerFS()
{}

QDir& OmActionLoggerFS::doGetLogFolder()
{
	zi::guard g(mutex_);
	if(!initialized){
		setupLogDir();
		initialized = true;
	}

	return mLogFolder;
}

void OmActionLoggerFS::setupLogDir()
{
	QString omniFolderName = ".omni";
	QString homeFolder = QDir::homePath();
	QString omniFolderPath = homeFolder + "/"
		+ omniFolderName + "/"
		+ "logFiles" + "/";

	QDir dir = QDir( omniFolderPath );
	if( dir.exists() ){
		mLogFolder = dir;
		return;
	}

	if( QDir::home().mkdir( omniFolderPath ) ){
		printf("made folder %s\n", qPrintable(omniFolderPath) );
		mLogFolder = dir;
	} else {
		const std::string errMsg =
			"could not make folder "+omniFolderPath.toStdString() + "\n";
		throw OmIoException(errMsg);
	}
}

QDataStream &operator<<(QDataStream & out, const OmSegmentValidateAction & a)
{
	int version = 1;
	out << version;

	OmSegIDsSet ids;
	std::set<OmSegment*>* segs = a.selectedSegments_.get();
	FOR_EACH(iter, *segs){
		OmSegment* seg = *iter;
		ids.insert(seg->value());
	}
	out << ids;
	out << a.valid_;
	out << a.mSegmentationId;

	return out;
}

QDataStream &operator>>(QDataStream & in, OmSegmentValidateAction & a)
{
	int version;
	in >> version;

	OmSegIDsSet ids;
	in >> ids;
	in >> a.valid_;
	in >> a.mSegmentationId;

	SegmentationDataWrapper sdw(a.mSegmentationId);
	OmSegmentCache* cache = sdw.getSegmentCache();
	std::set<OmSegment*>* segs = new std::set<OmSegment*>();
	FOR_EACH(iter, ids){
		OmSegment* seg = cache->GetSegment(*iter);
		segs->insert(seg);
	}

	a.selectedSegments_ = boost::shared_ptr<std::set<OmSegment*> >(segs);

	a.SetActivate(false);

	return in;
}

QDataStream &operator<<(QDataStream & out, const OmSegmentUncertainAction & a)
{
	int version = 1;
	out << version;

	OmSegIDsSet ids;
	std::set<OmSegment*>* segs = a.selectedSegments_.get();
	FOR_EACH(iter, *segs){
		OmSegment* seg = *iter;
		ids.insert(seg->value());
	}
	out << ids;
	out << a.uncertain_;
	out << a.mSegmentationId;

	return out;
}

QDataStream &operator>>(QDataStream & in, OmSegmentUncertainAction & a)
{
	int version;
	in >> version;

	OmSegIDsSet ids;
	in >> ids;
	in >> a.uncertain_;
	in >> a.mSegmentationId;

	SegmentationDataWrapper sdw(a.mSegmentationId);
	OmSegmentCache* cache = sdw.getSegmentCache();
	std::set<OmSegment*>* segs = new std::set<OmSegment*>();
	FOR_EACH(iter, ids){
		OmSegment* seg = cache->GetSegment(*iter);
		segs->insert(seg);
	}

	a.selectedSegments_ = boost::shared_ptr<std::set<OmSegment*> >(segs);

	a.SetActivate(false);

	return in;
}

QDataStream &operator<<(QDataStream & out, const OmSegmentSplitAction & a )
{
	int version = 1;
	out << version;
	out << a.mEdge;
	out << a.mSegmentationID;
	out << a.desc;

	return out;
}

QDataStream &operator>>(QDataStream & in,  OmSegmentSplitAction & a )
{
	int version;
	in >> version;
	in >> a.mEdge;
	in >> a.mSegmentationID;
	in >> a.desc;

	return in;
}

QDataStream &operator<<(QDataStream & out, const OmSegmentGroupAction & a )
{
	int version = 1;
	out << version;
	out << a.mSegmentationId;
	out << a.mName;
	out << a.mCreate;
	out << a.mSelectedSegmentIds;

	return out;
}

QDataStream &operator>>(QDataStream & in,  OmSegmentGroupAction & a )
{
	int version;
	in >> version;
	in >> a.mSegmentationId;
	in >> a.mName;
	in >> a.mCreate;
	in >> a.mSelectedSegmentIds;

	return in;
}

QDataStream &operator<<(QDataStream & out, const OmSegmentJoinAction & a )
{
	int version = 1;
	out << version;
	out << a.mSegmentationId;
	out << a.mSelectedSegmentIds;

	return out;
}

QDataStream &operator>>(QDataStream & in,  OmSegmentJoinAction & a )
{
	int version;
	in >> version;
	in >> a.mSegmentationId;
	in >> a.mSelectedSegmentIds;

	return in;
}

QDataStream &operator<<(QDataStream & out, const OmSegmentSelectAction & a )
{
	int version = 1;
	out << version;
	out << a.mSegmentationId;
	out << a.mNewSelectedIdSet;
	out << a.mOldSelectedIdSet;
	out << a.mSegmentJustSelectedID;

	return out;
}

QDataStream &operator>>(QDataStream & in,  OmSegmentSelectAction & a )
{
	int version;
	in >> version;
	in >> a.mSegmentationId;
	in >> a.mNewSelectedIdSet;
	in >> a.mOldSelectedIdSet;
	in >> a.mSegmentJustSelectedID;

	return in;
}

QDataStream &operator<<(QDataStream & out, const OmProjectSaveAction & )
{
	int version = 1;
	out << version;

	return out;
}

QDataStream &operator>>(QDataStream & in,   OmProjectSaveAction&  )
{
	int version;
	in >> version;

	return in;
}

QDataStream &operator<<(QDataStream & out, const OmSegmentationThresholdChangeAction & a )
{
	int version = 1;
	out << version;
	out << a.mThreshold;
	out << a.mOldThreshold;

	return out;
}

QDataStream &operator>>(QDataStream & in,  OmSegmentationThresholdChangeAction & a )
{
	int version;
	in >> version;
	in >> a.mThreshold;
	in >> a.mOldThreshold;

	return in;
}

QDataStream &operator<<(QDataStream & out, const OmVoxelSetValueAction & a )
{
	int version = 1;
	out << version;
	out << a.mSegmentationId;
	//out << a.mOldVoxelValues;	//FIXME
	out << a.mNewValue;

	return out;
}

QDataStream &operator>>(QDataStream & in,  OmVoxelSetValueAction & a )
{
	int version;
	in >> version;
	in >> a.mSegmentationId;
	//in >> a.mOldVoxelValues;	//FIXME
	in >> a.mNewValue;

	return in;
}

