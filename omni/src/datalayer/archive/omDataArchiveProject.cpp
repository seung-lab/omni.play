#include "common/omException.h"
#include "datalayer/archive/omDataArchiveBoost.h"
#include "datalayer/archive/omDataArchiveCoords.h"
#include "datalayer/archive/omDataArchiveProject.h"
#include "datalayer/archive/omDataArchiveVmml.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWriter.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentCacheImpl.h"
#include "segment/omSegmentEdge.h"
#include "system/omPreferences.h"
#include "system/omProjectData.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"

#include <QDataStream>

static const int Omni_Version = 7;
static const QString Omni_Postfix("OMNI");

void OmDataArchiveProject::ArchiveRead( const OmDataPath & path, OmProject * project ) 
{
	int size;
	OmDataWrapperPtr dw = OmProjectData::GetProjectDataReader()->dataset_raw_read(path, &size);
	
	QByteArray ba = QByteArray::fromRawData( dw->getCharPtr(), size );
	QDataStream in(&ba, QIODevice::ReadOnly);
	in.setByteOrder( QDataStream::LittleEndian );
	in.setVersion(QDataStream::Qt_4_6);

	int file_version;
	in >> file_version;

	if( Omni_Version != file_version ){
		throw OmIoException("can not open file: file version is (" 
				    + boost::lexical_cast<std::string>(file_version)
				    +"), but Omni expecting ("
				    + boost::lexical_cast<std::string>(Omni_Version) 
				    + ")");
	}

	in >> (*project);

	QString omniPostfix;
	in >> omniPostfix;

	if( Omni_Postfix != omniPostfix ){
		throw OmIoException("corruption detected in Omni file");
	}
}

void OmDataArchiveProject::ArchiveWrite( const OmDataPath & path, OmProject * project ) 
{
	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out.setByteOrder( QDataStream::LittleEndian );
	out.setVersion(QDataStream::Qt_4_6);

	out << Omni_Version;
	out << (*project);
	out << Omni_Postfix;

	OmProjectData::GetDataWriter()->dataset_raw_create_tree_overwrite( path, 
									   ba.size(), 
									   ba.data() );
}

QDataStream &operator<<(QDataStream & out, const OmProject & p)
{
	out << *OmPreferences::Instance();
	out << p.mChannelManager;
	out << p.mSegmentationManager;
	return out;
}

QDataStream &operator>>(QDataStream & in, OmProject & p)
{
	in >> *OmPreferences::Instance();

	in >> p.mChannelManager;
	in >> p.mSegmentationManager;

	return in;
}

QDataStream &operator<<(QDataStream & out, const OmPreferences & p )
{
	out << p.stringPrefs; 
	out << p.floatPrefs;
	out << p.intPrefs;
	out << p.boolPrefs;
	out << p.v3fPrefs;
	return out;
}

QDataStream &operator>>(QDataStream & in, OmPreferences & p )
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

QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmChannel> & cm )
{
	out << cm.mNextId;
	out << cm.mSize;
	out << cm.mValidSet;
	out << cm.mEnabledSet;

	foreach( const OmId & id, cm.mValidSet ){
		out << *cm.mMap[id];
	}

	return out;
}

QDataStream &operator>>(QDataStream & in, OmGenericManager<OmChannel> & cm )
{
	in >> cm.mNextId;
	in >> cm.mSize;
	in >> cm.mValidSet;
	in >> cm.mEnabledSet;

	cm.mMap.resize(cm.mSize, NULL);

	for( unsigned int i = 0; i < cm.mValidSet.size(); ++i ){
		OmChannel * chan = new OmChannel();
		in >> *chan;
		cm.mMap[ chan->GetId() ] = chan;
	}
	
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmChannel & chan )
{
	OmDataArchiveProject::storeOmManageableObject( out, chan );
	OmDataArchiveProject::storeOmMipVolume( out, chan );
	OmDataArchiveProject::storeOmVolume( out, chan );

	out << chan.mFilter2dManager;

	return out;
}

QDataStream &operator>>(QDataStream & in, OmChannel & chan )
{
	OmDataArchiveProject::loadOmManageableObject( in, chan );
	OmDataArchiveProject::loadOmMipVolume( in, chan );
	OmDataArchiveProject::loadOmVolume( in, chan );

	in >> chan.mFilter2dManager;

	return in;
}

/**
 * Filter
 */

QDataStream &operator<<(QDataStream & out, const OmFilter2dManager & fm )
{
	out << fm.mGenericFilterManager;
	return out;
}

QDataStream &operator>>(QDataStream & in, OmFilter2dManager & fm )
{
	in >> fm.mGenericFilterManager;
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmFilter2d> & fm )
{
	out << fm.mNextId;
	out << fm.mSize;
	out << fm.mValidSet;
	out << fm.mEnabledSet;

	foreach( const OmId & id, fm.mValidSet ){
		out << *fm.mMap[id];
	}

	return out;
}

QDataStream &operator>>(QDataStream & in, OmGenericManager<OmFilter2d> & fm )
{
	in >> fm.mNextId;
	in >> fm.mSize;
	in >> fm.mValidSet;
	in >> fm.mEnabledSet;

	fm.mMap.resize(fm.mSize, NULL);

	for( unsigned int i = 0; i < fm.mValidSet.size(); ++i ){
		OmFilter2d * filter = new OmFilter2d();
		in >> *filter;
		fm.mMap[ filter->GetId() ] = filter;
	}
	
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmFilter2d & f )
{
	OmDataArchiveProject::storeOmManageableObject( out, f );
	out << f.mAlpha;
	out << f.mChannel;
	out << f.mSeg;

	return out;
}

QDataStream &operator>>(QDataStream & in, OmFilter2d & f )
{
	OmDataArchiveProject::loadOmManageableObject( in, f );
	in >> f.mAlpha;
	in >> f.mChannel;
	in >> f.mSeg;

	return in;
}

/**
 * Segmentation and related
 */

QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmSegmentation> & sm )
{
	out << sm.mNextId;
	out << sm.mSize;
	out << sm.mValidSet;
	out << sm.mEnabledSet;

	foreach( const OmId & id, sm.mValidSet ){
		out << *sm.mMap[id];
	}

	return out;
}

QDataStream &operator>>(QDataStream & in, OmGenericManager<OmSegmentation> & sm )
{
	in >> sm.mNextId;
	in >> sm.mSize;
	in >> sm.mValidSet;
	in >> sm.mEnabledSet;

	sm.mMap.resize(sm.mSize, NULL);

	for( unsigned int i = 0; i < sm.mValidSet.size(); ++i ){
		OmSegmentation * seg = new OmSegmentation();
		in >> *seg;
		sm.mMap[ seg->GetId() ] = seg;
	}
	
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmSegmentation & seg )
{
	OmDataArchiveProject::storeOmManageableObject( out, seg );
	OmDataArchiveProject::storeOmVolume( out, seg );
	OmDataArchiveProject::storeOmMipVolume( out, seg );

	out << seg.mMipMeshManager;
	out << (*seg.mSegmentCache);

	out << seg.mDendSize;
	out << seg.mDendValuesSize;
	out << seg.mDendCount;
	out << seg.mDendThreshold;

	return out;
}

QDataStream &operator>>(QDataStream & in, OmSegmentation & seg )
{
	OmDataArchiveProject::loadOmManageableObject( in, seg );
	OmDataArchiveProject::loadOmVolume( in, seg );
	OmDataArchiveProject::loadOmMipVolume( in, seg );
 
	in >> seg.mMipMeshManager;
	in >> (*seg.mSegmentCache);

	in >> seg.mDendSize;
	in >> seg.mDendValuesSize;
	in >> seg.mDendCount;
	in >> seg.mDendThreshold;

        QString dendStr = QString("%1dend").arg(seg.GetDirectoryPath());
	OmDataPath path;
        path.setPathQstr(dendStr);

        if(OmProjectData::GetProjectDataReader()->dataset_exists(path)) {
		int size;
        	seg.mDend = OmProjectData::GetProjectDataReader()->dataset_raw_read(path, &size);
		assert( size == seg.mDendSize );

		QString dendValStr = QString("%1dendValues").arg(seg.GetDirectoryPath());
        	path.setPathQstr(dendValStr);
        	seg.mDendValues = OmProjectData::GetProjectDataReader()->dataset_raw_read(path, &size);
		assert( size == seg.mDendValuesSize );

		QString dendEdgeDisabledByUser = QString("%1/edgeDisabledByUser").arg(seg.GetDirectoryPath());
		path.setPathQstr(dendEdgeDisabledByUser);
		seg.mEdgeDisabledByUser = OmProjectData::GetProjectDataReader()->dataset_raw_read(path, &size);
		assert( size == seg.mDendValuesSize );

		// this is just a temporary object--should be refactored... (purcaro)
		quint8 * edgeJoined = (quint8 *)malloc(sizeof(quint8) * seg.mDendValuesSize );
		memset(edgeJoined, 0, sizeof(quint8) * seg.mDendValuesSize );
		seg.mEdgeWasJoined = OmDataWrapperPtr( new OmDataWrapper( edgeJoined ) );

		QString dendEdgeForceJoin = QString("%1/edgeForceJoin").arg(seg.GetDirectoryPath());
		path.setPathQstr(dendEdgeForceJoin);
		seg.mEdgeForceJoin = OmProjectData::GetProjectDataReader()->dataset_raw_read(path, &size);
		assert( size == seg.mDendValuesSize );
	}

	return in;
}

QDataStream &operator<<(QDataStream & out, const OmMipMeshManager & mm )
{
	out << mm.mDirectoryPath;

	return out;
}

QDataStream &operator>>(QDataStream & in, OmMipMeshManager & mm )
{
	in >> mm.mDirectoryPath;

	return in;
}
QDataStream &operator<<(QDataStream & out, const OmSegmentCache & sc )
{
        out << (*sc.mImpl);

	return out;
}

QDataStream &operator>>(QDataStream & in, OmSegmentCache & sc )
{
	in >> (*sc.mImpl);

	return in;
}

QDataStream &operator<<(QDataStream & out, const OmSegmentCacheImpl & sc )
{
	out << sc.validPageNumbers;

        out << sc.mAllSelected;
        out << sc.mAllEnabled;
        out << sc.mMaxValue;

        out << sc.mEnabledSet;
        out << sc.mSelectedSet;

	out << sc.segmentCustomNames;
	out << sc.segmentNotes;
	
	out << sc.mPageSize;
	out << sc.mNumSegs;
	out << sc.mNumTopLevelSegs;

	int size = sc.mManualUserMergeEdgeList.size();
	out << size;
	foreach( OmSegmentEdge * e, sc.mManualUserMergeEdgeList ){
		out << *e;
	}

	return out;
}

QDataStream &operator>>(QDataStream & in, OmSegmentCacheImpl & sc )
{
	in >> sc.validPageNumbers;

        in >> sc.mAllSelected;
        in >> sc.mAllEnabled;
        in >> sc.mMaxValue;

        in >> sc.mEnabledSet;
        in >> sc.mSelectedSet;

	in >> sc.segmentCustomNames;
	in >> sc.segmentNotes;

	in >> sc.mPageSize;
	in >> sc.mNumSegs;
	in >> sc.mNumTopLevelSegs;

	int size;
	in >> size;
	for( int i = 0; i < size; ++i ){
		OmSegmentEdge * e = new OmSegmentEdge();
		in >> (*e);
		sc.mManualUserMergeEdgeList.push_back(e);
	}

	return in;
}

QDataStream &operator<<(QDataStream & out, const OmSegmentEdge & se )
{
	out << se.parentID;
	out << se.childID;
	out << se.threshold;

	return out;
}

QDataStream &operator>>(QDataStream & in, OmSegmentEdge & se )
{
	in >> se.parentID;
	in >> se.childID;
	in >> se.threshold;

	return in;
}

/**
 * Generic base objects
 */

void OmDataArchiveProject::storeOmManageableObject( QDataStream & out, const OmManageableObject & mo )
{
	out << mo.mId;
	out << mo.mName;
	out << mo.mNote;
}

void OmDataArchiveProject::loadOmManageableObject( QDataStream & in, OmManageableObject & mo )
{
	in >> mo.mId;
	in >> mo.mName;
	in >> mo.mNote;
}

void OmDataArchiveProject::storeOmMipVolume( QDataStream & out, const OmMipVolume & m )
{
	out << m.mDirectoryPath;
	out << m.mMipLeafDim;
	out << m.mMipRootLevel;
	
	out << m.mSubsampleMode;
	out << m.mBuildState;
	out << m.mStoreChunkMetaData;

	out << m.mBytesPerSample;
}

void OmDataArchiveProject::loadOmMipVolume( QDataStream & in, OmMipVolume & m )
{
	in >> m.mDirectoryPath;
	in >> m.mMipLeafDim;
	in >> m.mMipRootLevel;
	
	in >> m.mSubsampleMode;
	in >> m.mBuildState;
	in >> m.mStoreChunkMetaData;

	in >> m.mBytesPerSample;
}

void OmDataArchiveProject::storeOmVolume( QDataStream & out, const OmVolume & v )
{
	out << v.mNormToSpaceMat;
	out << v.mNormToSpaceInvMat;
	out << v.mDataExtent;
	out << v.mDataResolution;
	out << v.mChunkDim;
	out << v.unitString;
	out << v.mDataStretchValues;
}

void OmDataArchiveProject::loadOmVolume( QDataStream & in, OmVolume & v )
{
	in >> v.mNormToSpaceMat;
	in >> v.mNormToSpaceInvMat;
	in >> v.mDataExtent;
	in >> v.mDataResolution;
	in >> v.mChunkDim;
	in >> v.unitString;
	in >> v.mDataStretchValues;
}
