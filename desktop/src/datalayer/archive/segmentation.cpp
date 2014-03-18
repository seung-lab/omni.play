#include "utility/yaml/omBaseTypes.hpp"
#include "datalayer/archive/segmentation.h"
#include "utility/yaml/mipVolume.hpp"
#include "segment/lowLevel/omPagingPtrStore.h"
#include "segment/lowLevel/omEnabledSegments.hpp"
#include "segment/lowLevel/omSegmentSelection.hpp"
#include "segment/io/omUserEdges.hpp"
#include "segment/omSegmentEdge.h"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "segment/omSegmentsImpl.h"
#include "segment/io/omValidGroupNum.hpp"
#include "project/details/omSegmentationManager.h"
#include "volume/omSegmentationLoader.h"
#include "utility/yaml/genericManager.hpp"
#include "datalayer/archive/dummy.hpp"

namespace YAMLold {

Emitter& operator<<(Emitter& out, const OmSegmentationManager& m) {
  out << BeginMap;
  genericManager::Save(out, m.manager_);
  out << EndMap;
  return out;
}

void operator>>(const Node& in, OmSegmentationManager& m) {
  genericManager::Load(in, m.manager_);
}

Emitter& operator<<(Emitter& out, const OmSegmentation& seg) {
  out << BeginMap;
  mipVolume<const OmSegmentation> volArchive(seg);
  volArchive.Store(out);

  out << Key << "Segments" << Value << (*seg.segments_);
  out << Key << "Num Edges" << Value << seg.mst_->numEdges_;

  DummyGroups dg;
  out << Key << "Groups" << Value << dg;

  out << EndMap;

  return out;
}

void operator>>(const Node& in, OmSegmentation& seg) {
  mipVolume<OmSegmentation> volArchive(seg);
  volArchive.Load(in);

  in["Segments"] >> (*seg.segments_);
  in["Num Edges"] >> seg.mst_->numEdges_;

  seg.LoadVolDataIfFoldersExist();

  seg.mst_->Read();
  seg.validGroupNum_->Load();
  seg.segments_->StartCaches();
  seg.segments_->refreshTree();
}

Emitter& operator<<(Emitter& out, const OmSegments& sc) {
  out << (*sc.impl_);

  return out;
}

void operator>>(const Node& in, OmSegments& sc) { in >> (*sc.impl_); }

Emitter& operator<<(Emitter& out, const OmSegmentsImpl& sc) {
  out << BeginMap;
  out << Key << "Num Segments" << Value << sc.mNumSegs;
  out << Key << "Max Value" << Value << sc.maxValue_.get();

  out << Key << "Enabled Segments" << Value << sc.enabledSegments_->enabled_;
  out << Key << "Selected Segments" << Value << sc.segmentSelection_->selected_;

  out << Key << "Segment Custom Names" << Value << sc.segmentCustomNames;
  out << Key << "Segment Notes" << Value << sc.segmentNotes;
  out << EndMap;
  return out;
}

void operator>>(const Node& in, OmSegmentsImpl& sc) {
  uint32_t maxValue;
  in["Num Segments"] >> sc.mNumSegs;
  in["Max Value"] >> maxValue;
  sc.maxValue_.set(maxValue);

  in["Enabled Segments"] >> sc.enabledSegments_->enabled_;
  in["Selected Segments"] >> sc.segmentSelection_->selected_;

  in["Segment Custom Names"] >> sc.segmentCustomNames;
  in["Segment Notes"] >> sc.segmentNotes;

  OmPagingPtrStore& segmentPages = *sc.store_->segmentPages_;
  segmentPages.Vol()->Loader()->LoadSegmentPages(segmentPages);

  OmUserEdges* userEdges = sc.segmentation_->MSTUserEdges();
  userEdges->Load();
}

Emitter& operator<<(Emitter& out, const OmSegmentEdge& se) {
  out << BeginMap;
  out << Key << "Parent Id" << Value << se.parentID;
  out << Key << "Child Id" << Value << se.childID;
  out << Key << "Threshold" << Value << se.threshold;
  out << EndMap;
  return out;
}

void operator>>(const Node& in, OmSegmentEdge& se) {
  in["Parent Id"] >> se.parentID;
  in["Child Id"] >> se.childID;
  in["Threshold"] >> se.threshold;
}

Emitter& operator<<(Emitter& out, const DummyGroups& g) {
  out << BeginMap;
  genericManager::Save(out, g.mGroupManager);
  out << Key << "Group Names" << Value << g.mGroupsByName;
  out << EndMap;
  return out;
}

Emitter& operator<<(Emitter& out, const DummyGroup& g) {
  out << BeginMap;
  out << Key << "Id" << Value << g.GetID();
  out << Key << "Note" << Value << g.GetNote();
  out << Key << "Custom Name" << Value << g.GetCustomName();
  out << Key << "Name" << Value << g.mName;
  out << Key << "Ids" << Value << g.mIDs;
  out << EndMap;
  return out;
}

}  // namespace YAMLold
