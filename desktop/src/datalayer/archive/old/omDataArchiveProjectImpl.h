#pragma once

#include <QString>
#include <QDataStream>

class OmChannel;
class OmChannelManager;
class OmDataPath;
class OmFilter2d;
class OmFilter2dManager;
class OmGroup;
class OmGroups;
class OmManageableObject;
class OmMeshManager;
class OmMipVolume;
class OmPagingPtrStore;
class OmPreferences;
class OmProject;
class OmProjectImpl;
class OmProjectVolumes;
class OmSegment;
class OmSegmentEdge;
class OmSegmentation;
class OmSegmentationManager;
class OmSegments;
class OmSegmentsImpl;
class OmVolume;

class OmDataArchiveProjectImpl {
public:
    // public for access by QDataStream operators

    static void LoadOldChannel(QDataStream& in, OmChannel& chan);
    static void LoadNewChannel(QDataStream& in, OmChannel& chan);

    static void LoadOldSegmentation(QDataStream& in, OmSegmentation& seg);
    static void LoadNewSegmentation(QDataStream& in, OmSegmentation& seg);

private:
    static void moveOldMeshMetadataFile(OmSegmentation* segmentation);
    static void rebuildSegments(OmSegmentation* vol);
};

QDataStream &operator>>(QDataStream& in, OmProjectImpl& project);
QDataStream &operator>>(QDataStream& in, OmProjectVolumes& p);
QDataStream &operator>>(QDataStream& in, OmPreferences& p);
QDataStream &operator>>(QDataStream& in, OmChannelManager&);
QDataStream &operator>>(QDataStream& in, OmChannel&);
QDataStream &operator>>(QDataStream& in, OmFilter2dManager&);
QDataStream &operator>>(QDataStream& in, OmFilter2d& f);
QDataStream &operator>>(QDataStream& in, OmSegmentationManager&);
QDataStream &operator>>(QDataStream& in, OmSegmentation& seg);
QDataStream &operator>>(QDataStream& in, OmMeshManager& mm);
QDataStream &operator>>(QDataStream& in, OmSegments& sc);
QDataStream &operator>>(QDataStream& in, OmSegmentsImpl& sc);
QDataStream &operator>>(QDataStream& in, OmPagingPtrStore& ps);
QDataStream &operator<<(QDataStream& out, const OmSegmentEdge& se);
QDataStream &operator>>(QDataStream& in, OmSegmentEdge& sc);
QDataStream &operator>>(QDataStream& in, OmGroups& g);

