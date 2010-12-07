#ifndef OM_DATA_ARCHIVE_PROJECT_H
#define OM_DATA_ARCHIVE_PROJECT_H

#include "common/omCommon.h"
#include "system/omGenericManager.h"
#include "system/omGroups.h"
#include "segment/lowLevel/omPagingPtrStore.h"

class OmSegment;
class OmDataPath;
class OmProject;
class OmManageableObject;
class OmVolume;
class OmMipVolume;
class OmPreferences;
class OmChannel;
class OmFilter2dManager;
class OmFilter2d;
class OmSegmentation;
class OmMipMeshManager;
class OmSegmentCache;
class OmSegmentCacheImpl;
class OmSegmentEdge;


class OmDataArchiveProject
{
 public:
	static void ArchiveRead( const OmDataPath & path, OmProject * project );
	static void ArchiveWrite( const OmDataPath & path, OmProject * project );

	static void storeOmManageableObject( QDataStream & out, const OmManageableObject & mo );
	static void loadOmManageableObject( QDataStream & in, OmManageableObject & mo );

	static void storeOmMipVolume( QDataStream & out, const OmMipVolume & m );
	static void loadOmMipVolume( QDataStream & in, OmMipVolume & m );

	static void storeOmVolume( QDataStream & out, const OmVolume & v );
	static void loadOmVolume( QDataStream & in, OmVolume & v );

private:
	static void Upgrade(const OmDataPath& path,
						OmProject* project);

};

QDataStream &operator<<(QDataStream & out, const OmProject & project );
QDataStream &operator>>(QDataStream & in, OmProject & project );

QDataStream &operator<<(QDataStream & out, const OmPreferences & p );
QDataStream &operator>>(QDataStream & in, OmPreferences & p );

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

QDataStream &operator<<(QDataStream & out, const OmPagingPtrStore & ps );
QDataStream &operator>>(QDataStream & in, OmPagingPtrStore & ps );

QDataStream &operator<<(QDataStream & out, const OmSegmentEdge & se );
QDataStream &operator>>(QDataStream & in, OmSegmentEdge & sc );

QDataStream &operator<<(QDataStream & out, const OmGroups & g );
QDataStream &operator>>(QDataStream & in, OmGroups & g );

QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmGroup> & );
QDataStream &operator>>(QDataStream & in, OmGenericManager<OmGroup> & );


#endif
