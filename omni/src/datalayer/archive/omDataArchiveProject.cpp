#include "mesh/omMipMeshManager.h"
#include "common/omException.h"
#include "datalayer/archive/omDataArchiveBoost.h"
#include "datalayer/archive/omDataArchiveCoords.h"
#include "datalayer/archive/omDataArchiveProject.h"
#include "datalayer/omIDataReader.h"
#include "datalayer/omIDataWriter.h"
#include "datalayer/omMST.h"
#include "datalayer/upgraders/omUpgraders.hpp"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentCacheImpl.h"
#include "segment/omSegmentEdge.h"
#include "system/omGroup.h"
#include "system/omGroups.h"
#include "system/omPreferences.h"
#include "system/omProjectData.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "volume/omVolumeData.hpp"

#include <QDataStream>

//TODO: Someday, delete subsamplemode and numtoplevel variables

static const int Omni_Version = 15;
static const QString Omni_Postfix("OMNI");
static int fileVersion_;

void OmDataArchiveProject::ArchiveRead(const OmDataPath& path,
									   OmProject* project)
{
	int size;

	OmDataWrapperPtr dw =
		OmProjectData::GetProjectIDataReader()->
		readDataset(path, &size);

	QByteArray ba = QByteArray::fromRawData(dw->getPtr<char>(), size);
	QDataStream in(&ba, QIODevice::ReadOnly);
	in.setByteOrder(QDataStream::LittleEndian);
	in.setVersion(QDataStream::Qt_4_6);

	in >> fileVersion_;
	OmProjectData::setFileVersion(fileVersion_);
	printf("Omni file version is %d\n", fileVersion_);

	if(fileVersion_ < 10){
		throw OmIoException("can not open file: file version is ("
							+ boost::lexical_cast<std::string>(fileVersion_)
							+"), but Omni expecting ("
							+ boost::lexical_cast<std::string>(Omni_Version)
							+ ")");
	}

	in >> (*project);

	QString omniPostfix;
	in >> omniPostfix;
	if(Omni_Postfix != omniPostfix ||
	   !in.atEnd()){
		throw OmIoException("corruption detected in Omni file");
	}

	if(fileVersion_ < 14){
		OmUpgraders::to15();
		OmUpgraders::to14();
		ArchiveWrite(path, project);
	} else if(fileVersion_ < 15){
		OmUpgraders::to15();
		ArchiveWrite(path, project);
	}
}

void OmDataArchiveProject::ArchiveWrite(const OmDataPath& path,
										OmProject* project)
{
	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out.setByteOrder(QDataStream::LittleEndian);
	out.setVersion(QDataStream::Qt_4_6);

	OmProjectData::setFileVersion(Omni_Version);

	out << Omni_Version;
	out << (*project);
	out << Omni_Postfix;

	OmProjectData::GetIDataWriter()->
		writeDataset( path,
			      ba.size(),
			      OmDataWrapperRaw(ba.constData()));

	OmProjectData::GetIDataWriter()->flush();
}

QDataStream &operator<<(QDataStream& out, const OmProject& p)
{
	out << OmPreferences::Instance();
	out << p.mChannelManager;
	out << p.mSegmentationManager;
	return out;
}

QDataStream &operator>>(QDataStream& in, OmProject& p)
{
	in >> OmPreferences::Instance();
	in >> p.mChannelManager;
	in >> p.mSegmentationManager;

	return in;
}

QDataStream &operator<<(QDataStream& out, const OmPreferences& p)
{
	out << p.stringPrefs;
	out << p.floatPrefs;
	out << p.intPrefs;
	out << p.boolPrefs;
	out << p.v3fPrefs;
	return out;
}

QDataStream &operator>>(QDataStream& in, OmPreferences& p)
{
	in >> p.stringPrefs;
	in >> p.floatPrefs;
	in >> p.intPrefs;
	in >> p.boolPrefs;
	in >> p.v3fPrefs;
	return in;
}

/**
 * Channel
 */

QDataStream &operator<<(QDataStream& out, const OmGenericManager<OmChannel>& cm)
{
	out << cm.mNextId;
	out << cm.mSize;
	out << cm.mValidSet;
	out << cm.mEnabledSet;

	foreach(const OmId& id, cm.mValidSet){
		out << *cm.mMap[id];
	}

	return out;
}

QDataStream &operator>>(QDataStream& in, OmGenericManager<OmChannel>& cm)
{
	in >> cm.mNextId;
	in >> cm.mSize;
	in >> cm.mValidSet;
	in >> cm.mEnabledSet;

	cm.mMap.resize(cm.mSize, NULL);

	for(unsigned int i = 0; i < cm.mValidSet.size(); ++i){
		OmChannel * chan = new OmChannel();
		in >> *chan;
		cm.mMap[ chan->GetID() ] = chan;
	}

	return in;
}

QDataStream &operator<<(QDataStream& out, const OmChannel& chan)
{
	OmDataArchiveProject::storeOmManageableObject(out, chan);
	OmDataArchiveProject::storeOmMipVolume(out, chan);
	OmDataArchiveProject::storeOmVolume(out, chan);

	out << chan.mFilter2dManager;
	out << chan.mWasBounded;

	float oldmax = 0; //TODO: delete
	float oldmin = 0; //TODO: delete
	out << oldmax;
	out << oldmin;

	return out;
}

QDataStream &operator>>(QDataStream& in, OmChannel& chan)
{
	OmDataArchiveProject::loadOmManageableObject(in, chan);
	OmDataArchiveProject::loadOmMipVolume(in, chan);
	OmDataArchiveProject::loadOmVolume(in, chan);

	in >> chan.mFilter2dManager;
	if(fileVersion_ > 13) {
		in >> chan.mWasBounded;

		float oldmax = 0; //TODO: delete
		float oldmin = 0; //TODO: delete
		in >> oldmax;
		in >> oldmin;
	} else{
		chan.mWasBounded = false;
	}

	if(fileVersion_ > 13){
		QDir filesDir = OmProjectData::GetFilesFolderPath();
		if(filesDir.exists()){
			chan.loadVolData();
		}
	}

	return in;
}

/**
 * Filter
 */

QDataStream &operator<<(QDataStream& out, const OmFilter2dManager& fm)
{
	out << fm.mGenericFilterManager;
	return out;
}

QDataStream &operator>>(QDataStream& in, OmFilter2dManager& fm)
{
	in >> fm.mGenericFilterManager;
	return in;
}

QDataStream &operator<<(QDataStream& out, const OmGenericManager<OmFilter2d>& fm)
{
	out << fm.mNextId;
	out << fm.mSize;
	out << fm.mValidSet;
	out << fm.mEnabledSet;

	foreach(const OmId& id, fm.mValidSet){
		out << *fm.mMap[id];
	}

	return out;
}

QDataStream &operator>>(QDataStream& in, OmGenericManager<OmFilter2d>& fm)
{
	in >> fm.mNextId;
	in >> fm.mSize;
	in >> fm.mValidSet;
	in >> fm.mEnabledSet;

	fm.mMap.resize(fm.mSize, NULL);

	for(unsigned int i = 0; i < fm.mValidSet.size(); ++i){
		OmFilter2d * filter = new OmFilter2d();
		in >> *filter;
		fm.mMap[ filter->GetID() ] = filter;
	}

	return in;
}

QDataStream &operator<<(QDataStream& out, const OmFilter2d& f)
{
	OmDataArchiveProject::storeOmManageableObject(out, f);
	out << f.mAlpha;
	out << f.mChannel;
	out << f.mSeg;

	return out;
}

QDataStream &operator>>(QDataStream& in, OmFilter2d& f)
{
	OmDataArchiveProject::loadOmManageableObject(in, f);
	in >> f.mAlpha;
	in >> f.mChannel;
	in >> f.mSeg;

	return in;
}

/**
 * Segmentation and related
 */

QDataStream &operator<<(QDataStream& out, const OmGenericManager<OmSegmentation>& sm)
{
	out << sm.mNextId;
	out << sm.mSize;
	out << sm.mValidSet;
	out << sm.mEnabledSet;

	foreach(const OmId& id, sm.mValidSet){
		out << *sm.mMap[id];
	}

	return out;
}

QDataStream &operator>>(QDataStream& in, OmGenericManager<OmSegmentation>& sm)
{
	in >> sm.mNextId;
	in >> sm.mSize;
	in >> sm.mValidSet;
	in >> sm.mEnabledSet;

	sm.mMap.resize(sm.mSize, NULL);

	for(unsigned int i = 0; i < sm.mValidSet.size(); ++i){
		OmSegmentation * seg = new OmSegmentation();
		in >> *seg;
		sm.mMap[ seg->GetID() ] = seg;
	}

	return in;
}

QDataStream &operator<<(QDataStream& out, const OmSegmentation& seg)
{
	OmDataArchiveProject::storeOmManageableObject(out, seg);
	OmDataArchiveProject::storeOmVolume(out, seg);
	OmDataArchiveProject::storeOmMipVolume(out, seg);

	out << (*seg.mMipMeshManager);
	out << (*seg.mSegmentCache);

	out << seg.mst_->mDendSize;
	out << seg.mst_->mDendValuesSize;
	out << seg.mst_->mDendCount;
	out << seg.mst_->mDendThreshold;
	out << (*seg.mGroups);

	return out;
}

QDataStream &operator>>(QDataStream& in, OmSegmentation& seg)
{
	OmDataArchiveProject::loadOmManageableObject(in, seg);
	OmDataArchiveProject::loadOmVolume(in, seg);
	OmDataArchiveProject::loadOmMipVolume(in, seg);

	in >> (*seg.mMipMeshManager);
	in >> (*seg.mSegmentCache);

	in >> seg.mst_->mDendSize;
	in >> seg.mst_->mDendValuesSize;
	in >> seg.mst_->mDendCount;
	in >> seg.mst_->mDendThreshold;
	in >> (*seg.mGroups);

	if(fileVersion_ > 13){
		QDir filesDir = OmProjectData::GetFilesFolderPath();
		if(filesDir.exists()){
			seg.loadVolData();
		}
	}

	seg.mst_->read(seg);
	seg.mSegmentCache->refreshTree();

	return in;
}

QDataStream &operator<<(QDataStream& out, const OmMipMeshManager& mm)
{
	out << mm.mDirectoryPath;

	return out;
}

QDataStream &operator>>(QDataStream& in, OmMipMeshManager& mm)
{
	in >> mm.mDirectoryPath;

	return in;
}

QDataStream &operator<<(QDataStream& out, const OmSegmentCache& sc)
{
	out << (*sc.mImpl);

	return out;
}

QDataStream &operator>>(QDataStream& in, OmSegmentCache& sc)
{
	in >> (*sc.mImpl);

	return in;
}

QDataStream &operator<<(QDataStream& out, const OmSegmentCacheImpl& sc)
{
	out << (*sc.mSegments);

	out << sc.mAllSelected;
	out << sc.mAllEnabled;
	out << sc.mMaxValue;

	out << sc.mEnabledSet;
	out << sc.mSelectedSet;

	out << sc.segmentCustomNames;
	out << sc.segmentNotes;

	out << sc.mNumSegs;

	int size = sc.mManualUserMergeEdgeList.size();
	out << size;
	foreach(const OmSegmentEdge& e, sc.mManualUserMergeEdgeList){
		out << e;
	}

	return out;
}

QDataStream &operator>>(QDataStream& in, OmSegmentCacheImpl& sc)
{
	in >> (*sc.mSegments);

	in >> sc.mAllSelected;
	in >> sc.mAllEnabled;
	in >> sc.mMaxValue;

	in >> sc.mEnabledSet;
	in >> sc.mSelectedSet;

	in >> sc.segmentCustomNames;
	in >> sc.segmentNotes;

	in >> sc.mNumSegs;

	if(fileVersion_ < 12) {
		quint32 mNumTopLevelSegs;
		in >> mNumTopLevelSegs;
	}

	int size;
	in >> size;
	for(int i = 0; i < size; ++i){
		OmSegmentEdge e;
		in >> e;
		if(0 == e.childID  ||
		    0 == e.parentID ||
		    std::isnan(e.threshold)){
			printf("warning: bad edge found: %d, %d, %f\n",
				   e.parentID,
				   e.childID,
				   e.threshold);
			continue;
		}
		e.valid = true;
		sc.mManualUserMergeEdgeList.push_back(e);
	}

	return in;
}

template< class T2 >
QDataStream &operator<<(QDataStream& out, const OmPagingPtrStore<T2>& ps)
{
	out << ps.validPageNumbers;
	out << ps.mPageSize;
	return out;
}

template< class T2 >
QDataStream &operator>>(QDataStream& in, OmPagingPtrStore<T2>& ps)
{
	in >> ps.validPageNumbers;
	in >> ps.mPageSize;
	return in;
}

QDataStream &operator<<(QDataStream& out, const OmSegmentEdge& se)
{
	out << se.parentID;
	out << se.childID;
	out << se.threshold;

	return out;
}

QDataStream &operator>>(QDataStream& in, OmSegmentEdge& se)
{
	in >> se.parentID;
	in >> se.childID;
	in >> se.threshold;


	return in;
}

/**
 * Generic base objects
 */

void OmDataArchiveProject::storeOmManageableObject(QDataStream& out, const OmManageableObject& mo)
{
	out << mo.mId;
	out << mo.mCustomName;
	out << mo.mNote;
}

void OmDataArchiveProject::loadOmManageableObject(QDataStream& in, OmManageableObject& mo)
{
	in >> mo.mId;
	in >> mo.mCustomName;
	in >> mo.mNote;
}

void OmDataArchiveProject::storeOmMipVolume(QDataStream& out, const OmMipVolume& m)
{
	out << QString(""); // TODO: remove me; was m.mDirectoryPath;
	out << m.mMipLeafDim;
	out << m.mMipRootLevel;

	qint32 subsamplemode = 0;
	out << subsamplemode;
	out << m.mBuildState;
	out << m.mStoreChunkMetaData;

	qint32 mBytesPerSample = 0;
	out << mBytesPerSample; //FIXME: no longer used; was mBytesPerSample

	const QString type =
		QString::fromStdString(OmVolumeTypeHelpers::GetTypeAsString(m.mVolDataType));
	out << type;
	std::cout << "saved type as " << type.toStdString() << "\n";
}

void OmDataArchiveProject::loadOmMipVolume(QDataStream& in, OmMipVolume& m)
{
	QString mDirectoryPath; //TODO: remove me
	in >> mDirectoryPath;
	in >> m.mMipLeafDim;
	in >> m.mMipRootLevel;

	qint32 subsamplemode;
	in >> subsamplemode;
	in >> m.mBuildState;
	in >> m.mStoreChunkMetaData;

	qint32 mBytesPerSample;
	in >> mBytesPerSample; //FIXME: no longer used

	if(fileVersion_ > 13){
		QString volDataType;
		in >> volDataType;
		m.mVolDataType = OmVolumeTypeHelpers::GetTypeFromString(volDataType);
	} else {
		m.mVolDataType = OmVolDataType::UNKNOWN;
	}
}

void OmDataArchiveProject::storeOmVolume(QDataStream& out,
										 const OmVolume& v)
{
	out << v.mNormToSpaceMat;
	out << v.mNormToSpaceInvMat;
	out << v.mDataExtent;
	out << v.mDataResolution;
	out << v.mChunkDim;
	out << v.unitString;
	out << v.mDataStretchValues;
}

void OmDataArchiveProject::loadOmVolume(QDataStream& in, OmVolume& v)
{
	in >> v.mNormToSpaceMat;
	in >> v.mNormToSpaceInvMat;
	in >> v.mDataExtent;
	in >> v.mDataResolution;
	in >> v.mChunkDim;
	in >> v.unitString;
	in >> v.mDataStretchValues;
}

QDataStream &operator<<(QDataStream& out, const OmGroups& g)
{
	out << g.mGroupManager;
	out << g.mGroupsByName;

	return out;
}

QDataStream &operator>>(QDataStream& in, OmGroups& g)
{
	in >> g.mGroupManager;
	in >> g.mGroupsByName;

	return in;
}

QDataStream &operator<<(QDataStream& out,
							const OmGenericManager<OmGroup>& gm)
{
	out << gm.mNextId;
	out << gm.mSize;
	out << gm.mValidSet;
	out << gm.mEnabledSet;

	foreach(const OmId& id, gm.mValidSet){
		out << *gm.mMap[id];
		printf("id=%i\n", id);
	}

	return out;
}

QDataStream &operator>>(QDataStream& in,
							OmGenericManager<OmGroup>& gm)
{
	in >> gm.mNextId;
	in >> gm.mSize;
	in >> gm.mValidSet;
	in >> gm.mEnabledSet;

	gm.mMap.resize(gm.mSize, NULL);

	for(unsigned int i = 0; i < gm.mValidSet.size(); ++i){
		OmGroup * group = new OmGroup();
		in >> *group;
		gm.mMap[ group->GetID() ] = group;
	}

	return in;
}

QDataStream &operator<<(QDataStream& out, const OmGroup& g)
{
	OmDataArchiveProject::storeOmManageableObject(out, g);
	out << g.mName;
	out << g.mIDs;

	return out;
}

QDataStream &operator>>(QDataStream& in, OmGroup& g)
{
	if(fileVersion_ > 11) {
		OmDataArchiveProject::loadOmManageableObject(in, g);
	}
	in >> g.mName;
	if(fileVersion_ > 11) {
		in >> g.mIDs;
	}

	return in;
}
