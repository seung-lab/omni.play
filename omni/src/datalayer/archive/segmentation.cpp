
#include "datalayer/archive/segmentation.h"
#include "datalayer/archive/genericManager.hpp"
#include "datalayer/archive/mipVolume.hpp"
#include "segment/lowLevel/omPagingPtrStore.h"
#include "segment/lowLevel/omEnabledSegments.hpp"
#include "segment/lowLevel/omSegmentSelection.hpp"
#include "segment/io/omUserEdges.hpp"
#include "segment/omSegmentEdge.h"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "segment/omSegmentsImpl.h"
#include "segment/io/omMST.h"
#include "segment/io/omValidGroupNum.hpp"
#include "project/details/omSegmentationManager.h"
#include "system/omGroups.h"
#include "system/omGroup.h"
#include "volume/omSegmentationLoader.h"

#include <QSet>

namespace om {
namespace data {
namespace archive {

YAML::Emitter &operator<<(YAML::Emitter & out, const OmSegmentationManager& m)
{
    out << YAML::BeginMap;
    genericManager::Save(out, m.manager_);
    out << YAML::EndMap;
    return out;
}

void operator>>(const YAML::Node& in, OmSegmentationManager& m)
{
    genericManager::Load(in, m.manager_);
}

YAML::Emitter &operator<<(YAML::Emitter& out, const OmSegmentation& seg)
{
    out << YAML::BeginMap;
    mipVolume<const OmSegmentation> volArchive(seg);
    volArchive.Store(out);
    
    out << YAML::Key << "Segments" << YAML::Value << (*seg.segments_);
    out << YAML::Key << "Num Edges" << YAML::Value << seg.mst_->numEdges_;
    out << YAML::Key << "Groups" << YAML::Value << (*seg.groups_);
    out << YAML::EndMap;
    
    return out;
}

void operator>>(const YAML::Node& in, OmSegmentation& seg)
{
    mipVolume<OmSegmentation> volArchive(seg);
    volArchive.Load(in);
    
    in["Segments"] >> (*seg.segments_);
    in["Num Edges"] >> seg.mst_->numEdges_;
    in["Groups"] >> (*seg.groups_);
    
    seg.LoadVolDataIfFoldersExist();
    
    seg.mst_->Read();
    seg.validGroupNum_->Load();
    seg.segments_->StartCaches();
    seg.segments_->refreshTree();
}

YAML::Emitter &operator<<(YAML::Emitter& out, const OmSegments& sc)
{
    out << (*sc.impl_);
    
    return out;
}

void operator>>(const YAML::Node& in, OmSegments& sc)
{
    in >> (*sc.impl_);
}

YAML::Emitter &operator<<(YAML::Emitter& out, const OmSegmentsImpl& sc)
{
    out << YAML::BeginMap;
    out << YAML::Key << "Num Segments" << YAML::Value << sc.mNumSegs;
    out << YAML::Key << "Max Value" << YAML::Value << sc.maxValue_.get();
    
    out << YAML::Key << "Enabled Segments" << YAML::Value << sc.enabledSegments_->enabled_;
    out << YAML::Key << "Selected Segments" << YAML::Value << sc.segmentSelection_->selected_;
    
    out << YAML::Key << "Segment Custom Names" << YAML::Value << sc.segmentCustomNames;
    out << YAML::Key << "Segment Notes" << YAML::Value << sc.segmentNotes;
    out << YAML::EndMap;
    return out;
}

void operator>>(const YAML::Node& in, OmSegmentsImpl& sc)
{
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

YAML::Emitter &operator<<(YAML::Emitter& out, const OmSegmentEdge& se)
{
    out << YAML::BeginMap;
    out << YAML::Key << "Parent Id" << YAML::Value << se.parentID;
    out << YAML::Key << "Child Id" << YAML::Value << se.childID;
    out << YAML::Key << "Threshold" << YAML::Value << se.threshold;
    out << YAML::EndMap;
    return out;
}

void operator>>(const YAML::Node& in, OmSegmentEdge& se)
{
    in["Parent Id"] >> se.parentID;
    in["Child Id"] >> se.childID;
    in["Threshold"] >> se.threshold;
}

YAML::Emitter &operator<<(YAML::Emitter& out, const OmGroups& g)
{
    out << YAML::BeginMap;
    genericManager::Save(out, g.mGroupManager);
    out << YAML::Key << "Group Names" << YAML::Value << g.mGroupsByName;
    out << YAML::EndMap;
    return out;
}

void operator>>(const YAML::Node& in, OmGroups& g)
{
    genericManager::Load(in, g.mGroupManager);
    in["Group Names"] >> g.mGroupsByName;
}

YAML::Emitter &operator<<(YAML::Emitter& out, const OmGroup& g)
{
    out << YAML::BeginMap;
    out << YAML::Key << "Id" << YAML::Value << g.GetID();
    out << YAML::Key << "Note" << YAML::Value << g.GetNote();
    out << YAML::Key << "Custom Name" << YAML::Value << g.GetCustomName();
    out << YAML::Key << "Name" << YAML::Value << g.mName;
    out << YAML::Key << "Ids" << YAML::Value << g.mIDs;
    out << YAML::EndMap;
    return out;
}

void operator>>(const YAML::Node& in, OmGroup& g)
{
    in["Id"] >> g.id_;
    in["Note"] >> g.note_;
    in["Custom Name"] >> g.customName_;
    in["Name"] >> g.mName;
    in["Ids"] >> g.mIDs;
}


}; // namespace archive
}; // namespace data
}; // namespace om