#include "datalayer/archive/omGenericManagerArchive.hpp"
#include "segment/lowLevel/omSegmentSelection.hpp"
#include "segment/lowLevel/omEnabledSegments.hpp"
#include "datalayer/archive/omMipVolumeArchive.h"
#include "datalayer/archive/omMipVolumeArchiveOld.h"
#include "common/omException.h"
#include "datalayer/archive/omDataArchiveBoost.h"
#include "datalayer/archive/omDataArchiveProject.h"
#include "datalayer/upgraders/omUpgraders.hpp"
#include "mesh/omMipMeshManagers.hpp"
#include "project/omProjectImpl.hpp"
#include "segment/io/omMST.h"
#include "segment/io/omUserEdges.hpp"
#include "segment/io/omValidGroupNum.hpp"
#include "segment/omSegments.h"
#include "segment/omSegmentsImpl.h"
#include "segment/omSegmentEdge.h"
#include "system/omGroup.h"
#include "system/omGroups.h"
#include "system/omPreferences.h"

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

    if(Omni_Postfix != omniPostfix || !in.atEnd()){
        throw OmIoException("corruption detected in Omni file");
    }

    if(fileVersion_ < Omni_Version){
        Upgrade(fnp, project);
    }

    FOR_EACH(iter, ChannelDataWrapper::ValidIDs())
    {
        const ChannelDataWrapper cdw(*iter);

        if(cdw.IsBuilt())
        {
            std::vector<OmFilter2d*> filters = cdw.GetFilters();

            FOR_EACH(fiter, filters)
            {
                OmFilter2d* filter = *fiter;
                filter->Load();
            }
        }
    }

    FOR_EACH(iter, SegmentationDataWrapper::ValidIDs())
    {
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

QDataStream &operator<<(QDataStream& out, const OmGenericManager<OmChannel>& m)
{
    OmGenericManagerArchive::Save(out, m);
    return out;
}

QDataStream &operator>>(QDataStream& in, OmGenericManager<OmChannel>& m)
{
    OmGenericManagerArchive::Load(in, m);
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

QDataStream &operator<<(QDataStream& out, const OmGenericManager<OmFilter2d>& m)
{
    OmGenericManagerArchive::Save(out, m);
    return out;
}

QDataStream &operator>>(QDataStream& in, OmGenericManager<OmFilter2d>& m)
{
    OmGenericManagerArchive::Load(in, m);
    return in;
}

QDataStream &operator<<(QDataStream& out, const OmFilter2d& f)
{
    OmMipVolumeArchiveOld::StoreOmManageableObject(out, f);
    out << f.alpha_;
    out << f.chanID_;
    out << f.segID_;

    return out;
}

QDataStream &operator>>(QDataStream& in, OmFilter2d& f)
{
    OmMipVolumeArchiveOld::LoadOmManageableObject(in, f);
    in >> f.alpha_;
    in >> f.chanID_;
    in >> f.segID_;

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

QDataStream &operator<<(QDataStream& out, const OmGenericManager<OmSegmentation>& m)
{
    OmGenericManagerArchive::Save(out, m);
    return out;
}

QDataStream &operator>>(QDataStream& in, OmGenericManager<OmSegmentation>& m)
{
    OmGenericManagerArchive::Load(in, m);
    return in;
}

QDataStream &operator<<(QDataStream& out, const OmSegmentation& seg)
{
    OmMipVolumeArchive::Store(out, seg);

    out << (*seg.segments_);
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

    in >> (*seg.segments_);

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
    seg.segments_->refreshTree();

    if(fileVersion_ < 23){
        OmDataArchiveProject::moveOldMeshMetadataFile(&seg);
    }
}

void OmDataArchiveProject::LoadNewSegmentation(QDataStream& in,
                                               OmSegmentation& seg)
{
    OmMipVolumeArchive::Load(in, seg);

    in >> (*seg.segments_);

    in >> seg.mst_->numEdges_;
    in >> seg.mst_->userThreshold_;
    in >> (*seg.groups_);

    seg.loadVolDataIfFoldersExist();

    seg.mst_->Read();
    seg.validGroupNum_->Load();
    seg.segments_->refreshTree();
}

QDataStream &operator<<(QDataStream& out, const OmSegments& sc)
{
    out << (*sc.mImpl);

    return out;
}

QDataStream &operator>>(QDataStream& in, OmSegments& sc)
{
    in >> (*sc.mImpl);

    return in;
}

QDataStream &operator<<(QDataStream& out, const OmSegmentsImpl& sc)
{
    out << (*sc.segmentPages_);

    out << false; // TODO: DEAD: was sc.segmentSelection_->allSelected_;
    out << false; //TODO: DEAD: was sc.mAllEnabled;
    out << sc.maxValue_.get();

    out << sc.enabledSegments_->enabled_;
    out << sc.segmentSelection_->selected_;

    out << sc.segmentCustomNames;
    out << sc.segmentNotes;

    out << sc.mNumSegs;

    return out;
}

QDataStream &operator>>(QDataStream& in, OmSegmentsImpl& sc)
{
    in >> (*sc.segmentPages_);

    bool dead;
    in >> dead;
    in >> dead;

    uint32_t maxValue;
    in >> maxValue;
    sc.maxValue_.set(maxValue);

    in >> sc.enabledSegments_->enabled_;
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
    QSet<PageNum> validPageNumbers;
    quint32 size;

    in >> validPageNumbers;
    in >> size;

    if(fileVersion_ < 17){
        ps.pageSize_ = size;

        FOR_EACH(iter, validPageNumbers){
            ps.validPageNums_.insert(*iter);
        }
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

QDataStream &operator<<(QDataStream& out, const OmGenericManager<OmGroup>& m)
{
    OmGenericManagerArchive::Save(out, m);
    return out;
}

QDataStream &operator>>(QDataStream& in, OmGenericManager<OmGroup>& m)
{
    OmGenericManagerArchive::Load(in, m);
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
