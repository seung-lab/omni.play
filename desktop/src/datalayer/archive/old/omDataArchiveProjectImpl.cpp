#include "common/common.h"
#include "common/colors.h"
#include "datalayer/archive/old/omDataArchiveBoost.h"
#include "datalayer/archive/old/omDataArchiveProjectImpl.h"
#include "datalayer/archive/old/omGenericManagerArchive.hpp"
#include "datalayer/archive/old/omMipVolumeArchive.h"
#include "datalayer/archive/old/omMipVolumeArchiveOld.h"
#include "mesh/omMeshManagers.hpp"
#include "project/omProject.h"
#include "project/omProjectImpl.hpp"
#include "segment/io/omUserEdges.hpp"
#include "segment/io/omValidGroupNum.hpp"
#include "segment/lowLevel/omEnabledSegments.hpp"
#include "segment/lowLevel/omPagingPtrStore.h"
#include "segment/lowLevel/omSegmentSelection.hpp"
#include "segment/omSegmentEdge.h"
#include "segment/omSegments.h"
#include "segment/omSegmentsImpl.h"
#include "system/omGenericManager.hpp"
#include "system/omGenericManager.hpp"
#include "system/omPreferences.h"
#include "volume/omSegmentationLoader.h"
#include "datalayer/archive/dummy.hpp"

#include <QSet>

QDataStream& operator>>(QDataStream& in, OmProjectImpl& p) {
  in >> OmPreferences::instance();
  in >> p.volumes_;

  return in;
}

QDataStream& operator>>(QDataStream& in, OmProjectVolumes& p) {
  in >> *p.channels_;
  in >> *p.segmentations_;

  return in;
}

QDataStream& operator>>(QDataStream& in, OmPreferences& p) {
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

QDataStream& operator>>(QDataStream& in, OmChannelManager& cm) {
  OmGenericManagerArchive::Load(in, cm.manager_);
  return in;
}

QDataStream& operator>>(QDataStream& in, OmChannel& chan) {
  if (OmProject::GetFileVersion() < 25) {
    OmDataArchiveProjectImpl::LoadOldChannel(in, chan);

  } else {
    OmDataArchiveProjectImpl::LoadNewChannel(in, chan);
  }

  return in;
}

void OmDataArchiveProjectImpl::LoadOldChannel(QDataStream& in,
                                              OmChannel& chan) {
  OmMipVolumeArchiveOld::Load(in, chan, OmProject::GetFileVersion());

  in >> chan.filterManager_;

  if (OmProject::GetFileVersion() > 13) {
    bool dead;
    in >> dead;

    if (OmProject::GetFileVersion() < 24) {
      float dead;
      in >> dead;
      in >> dead;
    }
  }

  if (OmProject::GetFileVersion() > 13) {
    chan.LoadVolDataIfFoldersExist();
  }
}

void OmDataArchiveProjectImpl::LoadNewChannel(QDataStream& in,
                                              OmChannel& chan) {
  OmMipVolumeArchive<OmChannel> volArchive(chan);
  volArchive.Load(in);

  in >> chan.filterManager_;
  chan.LoadVolDataIfFoldersExist();
}

/**
 * Filter
 */

QDataStream& operator>>(QDataStream& in, OmFilter2dManager& fm) {
  OmGenericManagerArchive::Load(in, fm.filters_);
  return in;
}
QDataStream& operator>>(QDataStream& in, OmFilter2d& f) {
  OmMipVolumeArchiveOld::LoadOmManageableObject(in, f);
  in >> f.alpha_;
  in >> f.chanID_;
  in >> f.segID_;

  return in;
}

/**
 * Segmentation and related
 */

QDataStream& operator>>(QDataStream& in, OmSegmentationManager& m) {
  OmGenericManagerArchive::Load(in, m.manager_);
  return in;
}

QDataStream& operator>>(QDataStream& in, OmSegmentation& seg) {
  if (OmProject::GetFileVersion() < 25) {
    OmDataArchiveProjectImpl::LoadOldSegmentation(in, seg);

  } else {
    OmDataArchiveProjectImpl::LoadNewSegmentation(in, seg);
  }

  return in;
}

void OmDataArchiveProjectImpl::LoadOldSegmentation(QDataStream& in,
                                                   OmSegmentation& seg) {
  OmMipVolumeArchiveOld::Load(in, seg, OmProject::GetFileVersion());

  if (OmProject::GetFileVersion() < 22) {
    QString dead;
    in >> dead;
  }

  in >> (*seg.segments_);

  if (OmProject::GetFileVersion() < 24) {
    int dead;
    in >> dead;
    in >> dead;
  }

  in >> seg.mst_->numEdges_;

  double dead;
  in >> dead;

  DummyGroups dg;
  in >> dg;

  if (OmProject::GetFileVersion() > 13) {
    seg.LoadVolDataIfFoldersExist();
  }

  if (OmProject::GetFileVersion() < 18) {
    seg.mst_->convert();
  }

  seg.mst_->Read();
  seg.validGroupNum_->Load();
  seg.segments_->StartCaches();
  seg.segments_->refreshTree();

  if (OmProject::GetFileVersion() < 23) {
    OmDataArchiveProjectImpl::moveOldMeshMetadataFile(&seg);
  }
}

void OmDataArchiveProjectImpl::moveOldMeshMetadataFile(OmSegmentation* vol) {
  const QString oldFileName = vol->Folder()->MeshMetadataFileOld();

  if (QFile::exists(oldFileName)) {
    const QString newFileName = vol->Folder()->MeshMetadataFilePerThreshold(1);

    OmFileHelpers::MoveFile(oldFileName, newFileName);
  }
}

void OmDataArchiveProjectImpl::LoadNewSegmentation(QDataStream& in,
                                                   OmSegmentation& seg) {
  OmMipVolumeArchive<OmSegmentation> volArchive(seg);
  volArchive.Load(in);

  in >> (*seg.segments_);

  in >> seg.mst_->numEdges_;

  double dead;
  in >> dead;

  DummyGroups dg;
  in >> dg;

  seg.LoadVolDataIfFoldersExist();

  seg.mst_->Read();
  seg.validGroupNum_->Load();
  seg.segments_->StartCaches();
  seg.segments_->refreshTree();
}

QDataStream& operator>>(QDataStream& in, OmSegments& sc) {
  in >> (*sc.impl_);

  return in;
}

QDataStream& operator>>(QDataStream& in, OmSegmentsImpl& sc) {
  OmPagingPtrStore* segmentPages = sc.store_->segmentPages_;
  in >> (*segmentPages);

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

  if (OmProject::GetFileVersion() < 12) {
    quint32 dead;
    in >> dead;
  }

  OmUserEdges* userEdges = sc.segmentation_->MSTUserEdges();

  if (OmProject::GetFileVersion() < 19) {
    int size;
    in >> size;
    for (int i = 0; i < size; ++i) {
      OmSegmentEdge e;
      in >> e;
      userEdges->AddEdgeFromProjectLoad(e);
    }
    userEdges->Save();
    log_infos << "loaded " << userEdges->Edges().size() << "user edges";
  } else {
    userEdges->Load();
  }

  return in;
}

QDataStream& operator>>(QDataStream& in, OmPagingPtrStore& ps) {
  QSet<om::common::PageNum> validPageNumbers;
  quint32 size;

  in >> validPageNumbers;
  in >> size;

  ps.Vol()->Loader()->LoadSegmentPages(ps, validPageNumbers, size);

  return in;
}

QDataStream& operator>>(QDataStream& in, OmSegmentEdge& se) {
  in >> se.parentID;
  in >> se.childID;
  in >> se.threshold;

  return in;
}

QDataStream& operator>>(QDataStream& in, DummyGroups& g) {
  OmGenericManagerArchive::Load(in, g.mGroupManager);
  in >> g.mGroupsByName;

  return in;
}

QDataStream& operator>>(QDataStream& in, DummyGroup& g) {
  if (OmProject::GetFileVersion() > 11) {
    OmMipVolumeArchiveOld::LoadOmManageableObject(in, g);
  }
  QString name;
  in >> name;
  g.mName = name;
  if (OmProject::GetFileVersion() > 11) {
    in >> g.mIDs;
  }

  return in;
}

QDataStream& operator<<(QDataStream& out, const OmSegmentEdge& se) {
  out << se.parentID;
  out << se.childID;
  out << se.threshold;

  return out;
}
