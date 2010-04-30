#ifndef OM_DATA_ARCHIVE_PROJECT_H
#define OM_DATA_ARCHIVE_PROJECT_H

#include "common/omDebug.h"
#include "project/omProject.h"
#include "system/omProjectData.h"
#include "segment/omSegmentCacheImpl.h"

class OmDataArchiveProject
{
 public:
	static void ArchiveRead( OmHdf5Path path, OmProject * project );
	static void ArchiveWrite( OmHdf5Path path, OmProject * project );

	static void storeOmManageableObject( QDataStream & out, const OmManageableObject & mo );
	static void loadOmManageableObject( QDataStream & in, OmManageableObject & mo );

	static void storeOmMipVolume( QDataStream & out, const OmMipVolume & m );
	static void loadOmMipVolume( QDataStream & in, OmMipVolume & m );
};

QDataStream &operator<<(QDataStream & out, const OmProject & project );
QDataStream &operator>>(QDataStream & in, OmProject & project );

QDataStream &operator<<(QDataStream & out, const OmPreferences & p );
QDataStream &operator>>(QDataStream & in, OmPreferences & p );

QDataStream &operator<<(QDataStream & out, const OmVolume & v );
QDataStream &operator>>(QDataStream & in, OmVolume & v );

QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmChannel> & );
QDataStream &operator>>(QDataStream & in, OmGenericManager<OmChannel> & );

QDataStream &operator<<(QDataStream & out, const OmChannel & );
QDataStream &operator>>(QDataStream & in, OmChannel & );

QDataStream &operator<<(QDataStream & out, const OmFilter2dManager & );
QDataStream &operator>>(QDataStream & in, OmFilter2dManager & );

QDataStream &operator<<(QDataStream & out, const OmFilter2d & f );
QDataStream &operator>>(QDataStream & in, OmFilter2d & f );

QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmFilter2d> & );
QDataStream &operator>>(QDataStream & in, OmGenericManager<OmFilter2d> & );

QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmSegmentation> & );
QDataStream &operator>>(QDataStream & in, OmGenericManager<OmSegmentation> & );

QDataStream &operator<<(QDataStream & out, const OmSegmentation & seg );
QDataStream &operator>>(QDataStream & in, OmSegmentation & seg );

QDataStream &operator<<(QDataStream & out, const OmMipMeshManager & mm );
QDataStream &operator>>(QDataStream & in, OmMipMeshManager & mm );

QDataStream &operator<<(QDataStream & out, const OmSegmentCache & sc );
QDataStream &operator>>(QDataStream & in, OmSegmentCache & sc );

QDataStream &operator<<(QDataStream & out, const OmSegmentCacheImpl & sc );
QDataStream &operator>>(QDataStream & in, OmSegmentCacheImpl & sc );

#endif
