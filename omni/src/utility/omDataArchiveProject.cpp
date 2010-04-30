#include "omDataArchiveProject.h"
#include "omDataArchiveVmml.h"
#include "omDataArchiveCoords.h"
#include <QDataStream>

void OmDataArchiveProject::ArchiveRead( OmHdf5Path path, OmProject * project ) 
{
	int size;
	char* p_data = (char*) OmProjectData::GetProjectDataReader()->dataset_raw_read(path, &size);
	
	QByteArray ba = QByteArray::fromRawData( p_data, size );
	QDataStream in(&ba, QIODevice::ReadOnly);
	in.setByteOrder( QDataStream::LittleEndian );
	in.setVersion(QDataStream::Qt_4_6);
	in >> (*project);

	delete p_data;
}

void OmDataArchiveProject::ArchiveWrite( OmHdf5Path path, OmProject * project ) 
{
	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out.setByteOrder( QDataStream::LittleEndian );
	out.setVersion(QDataStream::Qt_4_6);
	out << (*project);
	
	OmProjectData::GetDataWriter()->dataset_raw_create_tree_overwrite( path, 
									   ba.size(), 
									   ba.data() );
}

QDataStream &operator<<(QDataStream & out, const OmProject & )
{
	out << *OmPreferences::Instance();
	out << *OmVolume::Instance();
	return out;
}

QDataStream &operator>>(QDataStream & in, OmProject & )
{
	in >> *OmPreferences::Instance();
	in >> *OmVolume::Instance();

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

QDataStream &operator<<(QDataStream & out, const OmVolume & v )
{
	out << v.mNormToSpaceMat;
	out << v.mNormToSpaceInvMat;
	out << v.mDataExtent;
	out << v.mDataResolution;
	out << v.mChunkDim;
	out << v.unitString;

	out << v.mChannelManager;
	out << v.mSegmentationManager;

	return out;
}

QDataStream &operator>>(QDataStream & in, OmVolume & v )
{
	in >> v.mNormToSpaceMat;
	in >> v.mNormToSpaceInvMat;
	in >> v.mDataExtent;
	in >> v.mDataResolution;
	in >> v.mChunkDim;
	in >> v.unitString;

	in >> v.mChannelManager;
	in >> v.mSegmentationManager;

	return in;
}

/**
 * Channel
 */

QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmChannel> & cm )
{
	out << cm.mNextId;

	int numChannels = cm.mMap.size();
	out << numChannels;

	foreach( OmChannel * chan, cm.mMap ){
		out << *chan;
	}

	out << cm.mValidSet;
	out << cm.mEnabledSet;

	return out;
}

QDataStream &operator>>(QDataStream & in, OmGenericManager<OmChannel> & cm )
{
	in >> cm.mNextId;

	int numChannels;
	in >> numChannels;

	for( int i = 0; i < numChannels; ++i ){
		OmChannel * chan = new OmChannel();
		in >> *chan;
		cm.mMap[ chan->GetId() ] = chan;
	}

	in >> cm.mValidSet;
	in >> cm.mEnabledSet;
	
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmChannel & chan )
{
	OmDataArchiveProject::storeOmManageableObject( out, chan );
	OmDataArchiveProject::storeOmMipVolume( out, chan );

	out << chan.mFilter2dManager;

	return out;
}

QDataStream &operator>>(QDataStream & in, OmChannel & chan )
{
	OmDataArchiveProject::loadOmManageableObject( in, chan );
	OmDataArchiveProject::loadOmMipVolume( in, chan );

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

	int numFilters = fm.mMap.size();
	out << numFilters;

	foreach( OmFilter2d * filter, fm.mMap ){
		out << *filter;
	}

	out << fm.mValidSet;
	out << fm.mEnabledSet;

	return out;
}

QDataStream &operator>>(QDataStream & in, OmGenericManager<OmFilter2d> & fm )
{
	in >> fm.mNextId;

	int numFilters;
	in >> numFilters;

	for( int i = 0; i < numFilters; ++i ){
		OmFilter2d * filter = new OmFilter2d();
		in >> *filter;
		fm.mMap[ filter->GetId() ] = filter;
	}

	in >> fm.mValidSet;
	in >> fm.mEnabledSet;
	
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

	int numSegmentations = sm.mMap.size();
	out << numSegmentations;

	foreach( OmSegmentation * seg, sm.mMap ){
		out << *seg;
	}

	out << sm.mValidSet;
	out << sm.mEnabledSet;

	return out;
}

QDataStream &operator>>(QDataStream & in, OmGenericManager<OmSegmentation> & sm )
{
	in >> sm.mNextId;

	int numSegmentations;
	in >> numSegmentations;

	for( int i = 0; i < numSegmentations; ++i ){
		OmSegmentation * seg = new OmSegmentation();
		in >> *seg;
		sm.mMap[ seg->GetId() ] = seg;
	}

	in >> sm.mValidSet;
	in >> sm.mEnabledSet;
	
	return in;
}

QDataStream &operator<<(QDataStream & out, const OmSegmentation & seg )
{
	OmDataArchiveProject::storeOmManageableObject( out, seg );
	OmDataArchiveProject::storeOmMipVolume( out, seg );

	out << seg.mMipMeshManager;
	out << seg.mSegmentCache;

	return out;
}

QDataStream &operator>>(QDataStream & in, OmSegmentation & seg )
{
	OmDataArchiveProject::loadOmManageableObject( in, seg );
	OmDataArchiveProject::loadOmMipVolume( in, seg );

	in >> seg.mMipMeshManager;
	in >> seg.mSegmentCache;

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
        out << sc.mAllSelected;
        out << sc.mAllEnabled;
        out << sc.mMaxSegmentID;
        out << sc.mMaxValue;

        out << sc.mEnabledSet;
        out << sc.mSelectedSet;

	out << sc.segmentCustomNames;
	out << sc.segmentNotes;

	return out;
}

QDataStream &operator>>(QDataStream & in, OmSegmentCacheImpl & sc )
{
        in >> sc.mAllSelected;
        in >> sc.mAllEnabled;
        in >> sc.mMaxSegmentID;
        in >> sc.mMaxValue;

        in >> sc.mEnabledSet;
        in >> sc.mSelectedSet;

	in >> sc.segmentCustomNames;
	in >> sc.segmentNotes;

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
