#include "actions/io/omActionLoggerFS.h"
#include "datalayer/archive/omDataArchiveBoost.h"
#include "segment/omSegment.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

#include "actions/details/omSegmentGroupActionImpl.hpp"
#include "actions/details/omSegmentJoinActionImpl.hpp"
#include "actions/details/omSegmentSelectActionImpl.hpp"
#include "actions/details/omSegmentSplitActionImpl.hpp"
#include "actions/details/omSegmentValidateActionImpl.hpp"
#include "actions/details/omSegmentUncertainActionImpl.hpp"
#include "actions/details/omSegmentationThresholdChangeActionImpl.hpp"
#include "actions/details/omVoxelSetValueActionImpl.hpp"

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

	QDir dir = QDir( omniFolderPath);
	if( dir.exists()){
		mLogFolder = dir;
		return;
	}

	if( QDir::home().mkdir( omniFolderPath)){
		printf("made folder %s\n", qPrintable(omniFolderPath));
		mLogFolder = dir;
	} else {
		const std::string errMsg =
			"could not make folder "+omniFolderPath.toStdString() + "\n";
		throw OmIoException(errMsg);
	}
}

QDataStream& operator<<(QDataStream& out, const OmSegmentValidateActionImpl& a)
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

QDataStream& operator>>(QDataStream& in, OmSegmentValidateActionImpl& a)
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

	return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentUncertainActionImpl& a)
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

QDataStream& operator>>(QDataStream& in, OmSegmentUncertainActionImpl& a)
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

	return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentSplitActionImpl& a)
{
	int version = 1;
	out << version;
	out << a.mEdge;
	out << a.mSegmentationID;
	out << a.desc;

	return out;
}

QDataStream& operator>>(QDataStream& in,  OmSegmentSplitActionImpl& a)
{
	int version;
	in >> version;
	in >> a.mEdge;
	in >> a.mSegmentationID;
	in >> a.desc;

	return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentGroupActionImpl& a)
{
	int version = 1;
	out << version;
	out << a.mSegmentationId;
	out << a.mName;
	out << a.mCreate;
	out << a.mSelectedSegmentIds;

	return out;
}

QDataStream& operator>>(QDataStream& in,  OmSegmentGroupActionImpl& a)
{
	int version;
	in >> version;
	in >> a.mSegmentationId;
	in >> a.mName;
	in >> a.mCreate;
	in >> a.mSelectedSegmentIds;

	return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentJoinActionImpl& a)
{
	int version = 1;
	out << version;
	out << a.mSegmentationId;
	out << a.mSelectedSegmentIds;

	return out;
}

QDataStream& operator>>(QDataStream& in,  OmSegmentJoinActionImpl& a)
{
	int version;
	in >> version;
	in >> a.mSegmentationId;
	in >> a.mSelectedSegmentIds;

	return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentSelectActionImpl& a)
{
	int version = 1;
	out << version;
	out << a.mSegmentationId;
	out << a.mNewSelectedIdSet;
	out << a.mOldSelectedIdSet;
	out << a.mSegmentJustSelectedID;

	return out;
}

QDataStream& operator>>(QDataStream& in,  OmSegmentSelectActionImpl& a)
{
	int version;
	in >> version;
	in >> a.mSegmentationId;
	in >> a.mNewSelectedIdSet;
	in >> a.mOldSelectedIdSet;
	in >> a.mSegmentJustSelectedID;

	return in;
}

QDataStream& operator<<(QDataStream& out, const OmProjectSaveActionImpl&)
{
	int version = 1;
	out << version;

	return out;
}

QDataStream& operator>>(QDataStream& in,   OmProjectSaveActionImpl& )
{
	int version;
	in >> version;

	return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentationThresholdChangeActionImpl& a)
{
	int version = 1;
	out << version;
	out << a.mThreshold;
	out << a.mOldThreshold;

	return out;
}

QDataStream& operator>>(QDataStream& in,  OmSegmentationThresholdChangeActionImpl& a)
{
	int version;
	in >> version;
	in >> a.mThreshold;
	in >> a.mOldThreshold;

	return in;
}

QDataStream& operator<<(QDataStream& out, const OmVoxelSetValueActionImpl& a)
{
	int version = 1;
	out << version;
	out << a.mSegmentationId;
	//out << a.mOldVoxelValues;	//FIXME
	out << a.mNewValue;

	return out;
}

QDataStream& operator>>(QDataStream& in,  OmVoxelSetValueActionImpl& a)
{
	int version;
	in >> version;
	in >> a.mSegmentationId;
	//in >> a.mOldVoxelValues;	//FIXME
	in >> a.mNewValue;

	return in;
}

