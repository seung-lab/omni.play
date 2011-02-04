#include "segment/lowLevel/omSegmentSelection.hpp"
#include "datalayer/archive/omMipVolumeArchive.h"
#include "datalayer/archive/omMipVolumeArchiveOld.h"
#include "common/omException.h"
#include "datalayer/archive/omDataArchiveBoost.h"
#include "datalayer/archive/omDataArchiveProject.h"
#include "datalayer/upgraders/omUpgraders.hpp"
#include "mesh/omMipMeshManagers.hpp"
#include "project/omProject.h"
#include "project/omProjectImpl.hpp"
#include "project/omProjectVolumes.h"
#include "project/omSegmentationManager.h"
#include "segment/io/omMST.h"
#include "segment/io/omUserEdges.hpp"
#include "segment/io/omValidGroupNum.hpp"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentCacheImpl.h"
#include "segment/omSegmentEdge.h"
#include "system/omGroup.h"
#include "system/omGroups.h"
#include "system/omPreferences.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"

#include <QDataStream>

static const int Omni_Version = 25;
static const QString Omni_Postfix("OMNI");
static int fileVersion_;

void OmDataArchiveProject::ArchiveRead(const QString& fnp, OmProjectImpl* project)
{
    QFile file(fnp);
    if(!file.open(QIODevice::ReadOnly)) {
        throw OmIoException("could not open", fnp);
    }

    QDataStream in(&file);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_4_6);

    in >> fileVersion_;
    OmProject::setFileVersion(fileVersion_);
    printf("Omni file version is %d\n", fileVersion_);

    if(fileVersion_ < 10 || fileVersion_ > Omni_Version){
        const QString err =
            QString("can not open file: file version is (%1), but Omni expecting (%2)")
            .arg(fileVersion_)
            .arg(Omni_Version);
        throw OmIoException(err);
    }

    in >> (*project);

    QString omniPostfix;
    in >> omniPostfix;
    if(Omni_Postfix != omniPostfix ||
       !in.atEnd()){
        throw OmIoException("corruption detected in Omni file");
    }

    if(fileVersion_ < Omni_Version){
        Upgrade(fnp, project);
    }

    FOR_EACH(iter, OmProject::Volumes().Segmentations().GetValidSegmentationIds()){
        const SegmentationDataWrapper sdw(*iter);
        if(sdw.IsBuilt()){
            sdw.GetSegmentation().MeshManagers()->Load();
        }
    }
}

void OmDataArchiveProject::Upgrade(const QString& fnp, OmProjectImpl* project)
{
    if(fileVersion_ < 14){
        OmUpgraders::to14();
        OmUpgraders::to20();
        OmUpgraders::RebuildCenterOfSegmentData();
    } else if(fileVersion_ < 19){
        OmUpgraders::to20();
        OmUpgraders::RebuildCenterOfSegmentData();
    } else if(fileVersion_ < 20){
        OmUpgraders::to20();
    }

    ArchiveWrite(fnp, project);
}

void OmDataArchiveProject::moveOldMeshMetadataFile(OmSegmentation* segmentation)
{
    const QString oldFileName =
        OmFileNames::MeshMetadataFileOld(segmentation);

    QFile oldFile(oldFileName);
    if(oldFile.exists()){
        const QString newFileName =
            OmFileNames::MeshMetadataFilePerThreshold(segmentation, 1);

        OmFileHelpers::MoveFile(oldFileName, newFileName);
    }
}

void OmDataArchiveProject::ArchiveWrite(const QString& fnp, OmProjectImpl* project)
{
    QFile file(fnp);
    if(!file.open(QIODevice::WriteOnly)) {
        throw OmIoException("could not open", fnp);
    }

    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);
    out.setVersion(QDataStream::Qt_4_6);

    OmProject::setFileVersion(Omni_Version);

    out << Omni_Version;
    out << (*project);
    out << Omni_Postfix;
}

QDataStream &operator<<(QDataStream& out, const OmProjectImpl& p)
{
    out << OmPreferences::instance();
    out << p.volumes_;
    return out;
}

QDataStream &operator>>(QDataStream& in, OmProjectImpl& p)
{
    in >> OmPreferences::instance();
    in >> p.volumes_;

    return in;
}

QDataStream &operator<<(QDataStream& out, const OmProjectVolumes& p)
{
    out << *p.channels_;
    out << *p.segmentations_;
    return out;
}

QDataStream &operator>>(QDataStream& in, OmProjectVolumes& p)
{
    in >> *p.channels_;
    in >> *p.segmentations_;

    return in;
}

QDataStream &operator<<(QDataStream& out, const OmPreferences& p)
{
    out << p.stringPrefs_;
    out << p.floatPrefs_;
    out << p.intPrefs_;
    out << p.boolPrefs_;
    out << p.v3fPrefs_;
    return out;
}

QDataStream &operator>>(QDataStream& in, OmPreferences& p)
{
    in >> p.stringPrefs_;
    in >> p.floatPrefs_;
    in >> p.intPrefs_;
    in >> p.boolPrefs_;
    in >> p.v3fPrefs_;
    return in;
}

/**
 * Channel
 */

QDataStream &operator<<(QDataStream& out, const OmChannelManager& cm)
{
    out << cm.mChannelManager;
    return out;
}

QDataStream &operator>>(QDataStream& in, OmChannelManager& cm)
{
    in >> cm.mChannelManager;
    return in;
}

QDataStream &operator<<(QDataStream& out, const OmGenericManager<OmChannel>& cm)
{
    out << cm.mNextId;
    out << cm.mSize;
    out << cm.mValidSet;
    out << cm.mEnabledSet;

    foreach(const OmID& id, cm.mValidSet){
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

QDataStream& operator<<(QDataStream& out, const OmChannel& chan)
{
    OmMipVolumeArchive::Store(out, chan);

    out << chan.filterManager_;

    return out;
}

QDataStream& operator>>(QDataStream& in, OmChannel& chan)
{
    if(fileVersion_ < 25){
        OmDataArchiveProject::LoadOldChannel(in, chan);
    } else{
        OmDataArchiveProject::LoadNewChannel(in, chan);
    }

    return in;
}

void OmDataArchiveProject::LoadOldChannel(QDataStream& in, OmChannel& chan)
{
    OmMipVolumeArchiveOld::Load(in, chan, fileVersion_);

    in >> chan.filterManager_;

    if(fileVersion_ > 13) {
        bool dead;
        in >> dead;

        if( fileVersion_ < 24){
            float dead;
            in >> dead;
            in >> dead;
        }
    }

    if(fileVersion_ > 13){
        chan.loadVolDataIfFoldersExist();
    }
}

void OmDataArchiveProject::LoadNewChannel(QDataStream& in, OmChannel& chan)
{
    OmMipVolumeArchive::Load(in, chan);

    in >> chan.filterManager_;
    chan.loadVolDataIfFoldersExist();
}

/**
 * Filter
 */

QDataStream &operator<<(QDataStream& out, const OmFilter2dManager& fm)
{
    out << fm.filters_;
    return out;
}

QDataStream &operator>>(QDataStream& in, OmFilter2dManager& fm)
{
    in >> fm.filters_;
    return in;
}

QDataStream &operator<<(QDataStream& out, const OmGenericManager<OmFilter2d>& fm)
{
    out << fm.mNextId;
    out << fm.mSize;
    out << fm.mValidSet;
    out << fm.mEnabledSet;

    foreach(const OmID& id, fm.mValidSet){
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
    OmMipVolumeArchiveOld::StoreOmManageableObject(out, f);
    out << f.mAlpha;
    out << f.cdw_.GetChannelID();
    out << f.sdw_.GetSegmentationID();

    return out;
}

QDataStream &operator>>(QDataStream& in, OmFilter2d& f)
{
    OmMipVolumeArchiveOld::LoadOmManageableObject(in, f);
    in >> f.mAlpha;

    OmID channID;
    in >> channID;
    f.cdw_ = ChannelDataWrapper(channID);

    OmID segID;
    in >> segID;
    f.sdw_ = SegmentationDataWrapper(segID);

    return in;
}

/**
 * Segmentation and related
 */

QDataStream &operator<<(QDataStream & out, const OmSegmentationManager& m)
{
    out << m.mSegmentationManager;
    return out;
}

QDataStream &operator>>(QDataStream & in, OmSegmentationManager& m)
{
    in >> m.mSegmentationManager;
    return in;
}

QDataStream &operator<<(QDataStream& out, const OmGenericManager<OmSegmentation>& sm)
{
    out << sm.mNextId;
    out << sm.mSize;
    out << sm.mValidSet;
    out << sm.mEnabledSet;

    foreach(const OmID& id, sm.mValidSet){
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
    OmMipVolumeArchive::Store(out, seg);

    out << (*seg.segmentCache_);
    out << seg.mst_->numEdges_;
    out << seg.mst_->userThreshold_;
    out << (*seg.groups_);

    return out;
}

QDataStream &operator>>(QDataStream& in, OmSegmentation& seg)
{
    if(fileVersion_ < 25){
        OmDataArchiveProject::LoadOldSegmentation(in, seg);
    } else {
        OmDataArchiveProject::LoadNewSegmentation(in, seg);
    }

    return in;
}

void OmDataArchiveProject::LoadOldSegmentation(QDataStream& in,
                                               OmSegmentation& seg)
{
    OmMipVolumeArchiveOld::Load(in, seg, fileVersion_);

    if(fileVersion_ < 22){
        QString dead;
        in >> dead;
    }

    in >> (*seg.segmentCache_);

    if(fileVersion_ < 24){
        int dead;
        in >> dead;
        in >> dead;
    }

    in >> seg.mst_->numEdges_;
    in >> seg.mst_->userThreshold_;
    in >> (*seg.groups_);

    if(fileVersion_ > 13){
        seg.loadVolDataIfFoldersExist();
    }

    if(fileVersion_ < 18){
        seg.mst_->convert();
    }

    seg.mst_->Read();
    seg.validGroupNum_->Load();
    seg.segmentCache_->refreshTree();

    if(fileVersion_ < 23){
        OmDataArchiveProject::moveOldMeshMetadataFile(&seg);
    }
}

void OmDataArchiveProject::LoadNewSegmentation(QDataStream& in,
                                               OmSegmentation& seg)
{
    OmMipVolumeArchive::Load(in, seg);

    in >> (*seg.segmentCache_);

    in >> seg.mst_->numEdges_;
    in >> seg.mst_->userThreshold_;
    in >> (*seg.groups_);

    seg.loadVolDataIfFoldersExist();

    seg.mst_->Read();
    seg.validGroupNum_->Load();
    seg.segmentCache_->refreshTree();
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

    out << false; // TODO: DEAD: was sc.segmentSelection_->allSelected_;
    out << sc.mAllEnabled;
    out << sc.mMaxValue;

    out << sc.mEnabledSet;
    out << sc.segmentSelection_->selected_;

    out << sc.segmentCustomNames;
    out << sc.segmentNotes;

    out << sc.mNumSegs;

    return out;
}

QDataStream &operator>>(QDataStream& in, OmSegmentCacheImpl& sc)
{
    in >> (*sc.mSegments);

    bool dead;
    in >> dead;
    in >> sc.mAllEnabled;
    in >> sc.mMaxValue;

    in >> sc.mEnabledSet;
    in >> sc.segmentSelection_->selected_;

    in >> sc.segmentCustomNames;
    in >> sc.segmentNotes;

    in >> sc.mNumSegs;

    if(fileVersion_ < 12) {
        quint32 dead;
        in >> dead;
    }

    OmUserEdges* userEdges = sc.segmentation_->MSTUserEdges();

    if(fileVersion_ < 19){
        int size;
        in >> size;
        for(int i = 0; i < size; ++i){
            OmSegmentEdge e;
            in >> e;
            userEdges->AddEdgeFromProjectLoad(e);
        }
        userEdges->Save();
        printf("loaded %d user edges\n", userEdges->Edges().size());
    } else {
        userEdges->Load();
    }

    return in;
}

QDataStream &operator<<(QDataStream& out, const OmPagingPtrStore&)
{
    QSet<PageNum> nums;
    quint32 size = 0;

    out << nums;
    out << size;
    return out;
}

QDataStream &operator>>(QDataStream& in, OmPagingPtrStore& ps)
{
    QSet<PageNum> nums;
    quint32 size;

    in >> nums;
    in >> size;

    if(fileVersion_ < 17){
        ps.pageSize_ = size;
        ps.validPageNumbers_ = nums;
        ps.storeMetadata();
    }

    if(ps.segmentation_->IsBuilt()){
        ps.loadAllSegmentPages();
    }

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

    foreach(const OmID& id, gm.mValidSet){
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
    OmMipVolumeArchiveOld::StoreOmManageableObject(out, g);
    out << g.mName;
    out << g.mIDs;

    return out;
}

QDataStream &operator>>(QDataStream& in, OmGroup& g)
{
    if(fileVersion_ > 11) {
        OmMipVolumeArchiveOld::LoadOmManageableObject(in, g);
    }
    in >> g.mName;
    if(fileVersion_ > 11) {
        in >> g.mIDs;
    }

    return in;
}
