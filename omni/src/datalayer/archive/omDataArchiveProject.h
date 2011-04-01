#ifndef OM_DATA_ARCHIVE_PROJECT_H
#define OM_DATA_ARCHIVE_PROJECT_H

#include "common/omCommon.h"
#include "system/omGenericManager.hpp"
#include "system/omGroups.h"

class OmPagingPtrStore;
class OmProjectVolumes;
class OmChannel;
class OmChannelManager;
class OmDataPath;
class OmFilter2d;
class OmFilter2dManager;
class OmManageableObject;
class OmMipMeshManager;
class OmMipVolume;
class OmPreferences;
class OmProject;
class OmProjectImpl;
class OmSegment;
class OmSegmentEdge;
class OmSegmentation;
class OmSegmentationManager;
class OmSegments;
class OmSegmentsImpl;
class OmVolume;

class OmDataArchiveProject {
public:
    static void ArchiveRead(const QString& fnp, OmProjectImpl* project);
    static void ArchiveWrite(const QString& fnp, OmProjectImpl* project );

    static void moveOldMeshMetadataFile(OmSegmentation* segmentation);

    // public for access by QDataStream operators
    static void LoadOldChannel(QDataStream& in, OmChannel& chan);
    static void LoadNewChannel(QDataStream& in, OmChannel& chan);

    static void LoadOldSegmentation(QDataStream& in, OmSegmentation& seg);
    static void LoadNewSegmentation(QDataStream& in, OmSegmentation& seg);

private:
    static void Upgrade(const QString& fnp, OmProjectImpl* project);
};

QDataStream &operator<<(QDataStream & out, const OmProject & project );
QDataStream &operator>>(QDataStream & in, OmProject & project );

QDataStream &operator<<(QDataStream& out, const OmProjectVolumes& p);
QDataStream &operator>>(QDataStream& in, OmProjectVolumes& p);

QDataStream &operator<<(QDataStream & out, const OmPreferences & p );
QDataStream &operator>>(QDataStream & in, OmPreferences & p );

QDataStream &operator<<(QDataStream & out, const OmChannelManager& );
QDataStream &operator>>(QDataStream & in, OmChannelManager& );
QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmChannel>&);
QDataStream &operator>>(QDataStream & in, OmGenericManager<OmChannel>&);

QDataStream &operator<<(QDataStream & out, const OmChannel & );
QDataStream &operator>>(QDataStream & in, OmChannel & );

QDataStream &operator<<(QDataStream & out, const OmFilter2dManager & );
QDataStream &operator>>(QDataStream & in, OmFilter2dManager & );

QDataStream &operator<<(QDataStream & out, const OmFilter2d & f );
QDataStream &operator>>(QDataStream & in, OmFilter2d & f );

QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmFilter2d> & );
QDataStream &operator>>(QDataStream & in, OmGenericManager<OmFilter2d> & );

QDataStream &operator<<(QDataStream & out, const OmSegmentationManager&);
QDataStream &operator>>(QDataStream & in, OmSegmentationManager&);
QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmSegmentation> & );
QDataStream &operator>>(QDataStream & in, OmGenericManager<OmSegmentation> & );

QDataStream &operator<<(QDataStream & out, const OmSegmentation & seg );
QDataStream &operator>>(QDataStream & in, OmSegmentation & seg );

QDataStream &operator<<(QDataStream & out, const OmMipMeshManager & mm );
QDataStream &operator>>(QDataStream & in, OmMipMeshManager & mm );

QDataStream &operator<<(QDataStream & out, const OmSegments & sc );
QDataStream &operator>>(QDataStream & in, OmSegments & sc );

QDataStream &operator<<(QDataStream & out, const OmSegmentsImpl & sc );
QDataStream &operator>>(QDataStream & in, OmSegmentsImpl & sc );

QDataStream &operator<<(QDataStream & out, const OmPagingPtrStore & ps );
QDataStream &operator>>(QDataStream & in, OmPagingPtrStore & ps );

QDataStream &operator<<(QDataStream & out, const OmSegmentEdge & se );
QDataStream &operator>>(QDataStream & in, OmSegmentEdge & sc );

QDataStream &operator<<(QDataStream & out, const OmGroups & g );
QDataStream &operator>>(QDataStream & in, OmGroups & g );

QDataStream &operator<<(QDataStream & out, const OmGenericManager<OmGroup> & );
QDataStream &operator>>(QDataStream & in, OmGenericManager<OmGroup> & );


#endif
