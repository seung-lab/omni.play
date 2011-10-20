
#include "datalayer/archive/segmentation.h"
#include "datalayer/archive/volume.hpp"
//#include "segment/segment.h"
//#include "segment/segments.h"
//#include "segment/segmentsImpl.h"
#include "project/details/segmentationManager.h"
//#include "volume/segmentationLoader.h"
#include "utility/yaml/genericManager.hpp"

namespace YAML {

Emitter &operator<<(Emitter & out, const om::proj::segmentationManager& m)
{
    out << BeginMap;
    genericManager::Save(out, m.manager_);
    out << EndMap;
    return out;
}

void operator>>(const Node& in, om::proj::segmentationManager& m) {
    genericManager::Load(in, m.manager_);
}

Emitter &operator<<(Emitter& out, const om::volume::segmentation& seg)
{
    out << BeginMap;
    volume<const om::volume::segmentation> volArchive(seg);
    volArchive.Store(out);

//    out << Key << "Segments" << Value << (*seg.segments_);
//    out << Key << "Num Edges" << Value << seg.mst_->numEdges_;
//    out << Key << "Groups" << Value << (*seg.groups_);
    out << EndMap;

    return out;
}

void operator>>(const Node& in, om::volume::segmentation& seg)
{
    volume<om::volume::segmentation> volArchive(seg);
    volArchive.Load(in);

//    in["Segments"] >> (*seg.segments_);
//    in["Num Edges"] >> seg.mst_->numEdges_;
//    in["Groups"] >> (*seg.groups_);

    seg.LoadVolDataIfFoldersExist();
}
/*
Emitter &operator<<(Emitter& out, const segments& sc)
{
    out << (*sc.impl_);

    return out;
}

void operator>>(const Node& in, segments& sc)
{
    in >> (*sc.impl_);
}

Emitter &operator<<(Emitter& out, const segmentsImpl& sc)
{
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

void operator>>(const Node& in, segmentsImpl& sc)
{
    uint32_t maxValue;
    in["Num Segments"] >> sc.mNumSegs;
    in["Max Value"] >> maxValue;
    sc.maxValue_.set(maxValue);

    in["Enabled Segments"] >> sc.enabledSegments_->enabled_;
    in["Selected Segments"] >> sc.segmentSelection_->selected_;

    in["Segment Custom Names"] >> sc.segmentCustomNames;
    in["Segment Notes"] >> sc.segmentNotes;

    pagingPtrStore& segmentPages = *sc.store_->segmentPages_;
    segmentPages.Vol()->Loader()->LoadSegmentPages(segmentPages);

    OmUserEdges* userEdges = sc.segmentation_->MSTUserEdges();
    userEdges->Load();
}

Emitter &operator<<(Emitter& out, const segmentEdge& se)
{
    out << BeginMap;
    out << Key << "Parent Id" << Value << se.parentID;
    out << Key << "Child Id" << Value << se.childID;
    out << Key << "Threshold" << Value << se.threshold;
    out << EndMap;
    return out;
}

void operator>>(const Node& in, segmentEdge& se)
{
    in["Parent Id"] >> se.parentID;
    in["Child Id"] >> se.childID;
    in["Threshold"] >> se.threshold;
}

Emitter &operator<<(Emitter& out, const OmGroups& g)
{
    out << BeginMap;
    genericManager::Save(out, g.mGroupManager);
    out << Key << "Group Names" << Value << g.mGroupsByName;
    out << EndMap;
    return out;
}

void operator>>(const Node& in, OmGroups& g)
{
    genericManager::Load(in, g.mGroupManager);
    in["Group Names"] >> g.mGroupsByName;
}

Emitter &operator<<(Emitter& out, const OmGroup& g)
{
    out << BeginMap;
    out << Key << "Id" << Value << g.GetID();
    out << Key << "Note" << Value << g.GetNote();
    out << Key << "Custom Name" << Value << g.GetCustomName();
    out << Key << "Name" << Value << g.mName;
    out << Key << "Ids" << Value << g.mIDs;
    out << EndMap;
    return out;
}

void operator>>(const Node& in, OmGroup& g)
{
    in["Id"] >> g.id_;
    in["Note"] >> g.note_;
    in["Custom Name"] >> g.customName_;
    in["Name"] >> g.mName;
    in["Ids"] >> g.mIDs;
}
*/

} // namespace YAML
