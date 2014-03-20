#include "project/omProjectGlobals.h"
#include "segment/lists/omSegmentLists.h"
#include "segment/lowLevel/children.hpp"
#include "segment/lowLevel/graphThresholder.hpp"
#include "segment/lowLevel/store.h"
#include "segment/omSegmentsImpl.h"
#include "segment/selection.hpp"
#include "segment/userEdgeVector.hpp"
#include "system/cache/omCacheManager.h"
#include "utility/omRandColorFile.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "volume/metadataManager.h"

using namespace om::segment;

// entry into this class via OmSegments hopefully guarantees proper locking...

OmSegmentsImpl::OmSegmentsImpl(SegDataVector& data, SegListDataVector& listData,
                               EdgeVector& mst, UserEdgeVector& userEdges,
                               om::volume::MetadataManager& meta,
                               OmValidGroupNum& valid,
                               SegmentationDataWrapper sdw)
    : mst_(mst),
      userEdges_(userEdges),
      meta_(meta),
      store_(new Store(data, listData, meta.coordSystem(), meta.maxSegments())),
      graph_(new Graph(meta.maxSegments())),
      segmentLists_(new OmSegmentLists(meta_, *store_, sdw)),
      selection_(new om::segment::Selection(*graph_, *store_, *segmentLists_)),
      thresholder_(new om::segment::GraphThresholder(*graph_, valid,
                                                     *segmentLists_, *store_,
                                                     mst_)) {

  thresholder_->SetGlobalThreshold();
}

OmSegmentsImpl::~OmSegmentsImpl() {}

OmSegment* OmSegmentsImpl::AddSegment() {
  const auto newValue = meta_.MaxSegmentsInc();

  assert(newValue);

  OmSegment* newSeg = AddSegment(newValue);
  return newSeg;
}

OmSegment* OmSegmentsImpl::AddSegment(const om::common::SegID value) {
  if (0 == value) {
    return nullptr;
  }

  OmSegment* seg = store_->AddSegment(value);
  seg->SetColor(OmProject::Globals().RandColorFile().GetRandomColor(value));
  if (value > meta_.maxSegments()) {
    graph_->Resize(value);
    meta_.UpdateMaxSegments(value);
  }

  return seg;
}

OmSegment* OmSegmentsImpl::GetOrAddSegment(const om::common::SegID val) {
  if (0 == val) {
    return nullptr;
  }

  OmSegment* seg = store_->GetSegment(val);

  if (nullptr == seg) {
    seg = AddSegment(val);
  }

  return seg;
}

om::segment::UserEdge OmSegmentsImpl::SplitEdgeUserAction(
    const om::segment::UserEdge& e) {
  boost::optional<std::string> splittableTest = IsEdgeSplittable(e);
  if (splittableTest) {
    log_debugs << "Split error: " << *splittableTest;
    return om::segment::UserEdge();
  }

  OmSegment* child = store_->GetSegment(e.childID);

  return splitChildFromParentNoTest(child);
}

boost::optional<std::string> OmSegmentsImpl::IsEdgeSplittable(
    const om::segment::UserEdge& e) {
  if (!e.valid) {
    return std::string("invalid edge");
  }

  OmSegment* child = store_->GetSegment(e.childID);

  return IsSegmentSplittable(child);
}

boost::optional<std::string> OmSegmentsImpl::IsSegmentSplittable(
    OmSegment* child) {
  if (!child->getParent()) {
    return std::string("segment is the root");
  }

  OmSegment* parent = child->getParent();
  assert(parent);

  if (child->IsValidListType() == parent->IsValidListType() &&
      1 == child->IsValidListType()) {
    return std::string("segments are valid");
  }

  return false;
}

boost::optional<std::string> OmSegmentsImpl::IsSegmentCuttable(OmSegment* seg) {
  if (seg->IsValidListType()) {
    return std::string("segment is valid");
  }

  return false;
}

om::segment::UserEdge OmSegmentsImpl::splitChildFromParentNoTest(
    OmSegment* child) {
  OmSegment* parent = child->getParent();

  auto edgeThatGotBroken = om::segment::UserEdge{
      parent->value(), child->value(), child->getThreshold(), true};

  graph_->SegChildren().RemoveChild(parent, child);
  graph_->Cut(child->value());
  child->setParent(nullptr);
  child->setThreshold(0);

  auto parentRoot = FindRoot(parent);
  parentRoot->touchFreshnessForMeshes();
  child->touchFreshnessForMeshes();

  Selection().setSegmentSelected(
      child->value(), Selection().IsSegmentSelected(parent->value()), true);

  if (-1 != child->getEdgeNumber()) {
    const int e = child->getEdgeNumber();

    mst_[e].userSplit = 1;
    mst_[e].wasJoined = 0;
    mst_[e].userJoin = 0;
    child->setEdgeNumber(-1);
  }

  if (child->getCustomMergeEdge().valid) {
    userEdges_.erase(std::find(userEdges_.cbegin(), userEdges_.cend(),
                               child->getCustomMergeEdge()));
    child->setCustomMergeEdge(om::segment::UserEdge());
  }

  const SizeAndNumPieces childInfo =
      graph_->SegChildren().ComputeSegmentSizeWithChildren(child);

  segmentLists_->UpdateSizeListsFromSplit(parentRoot, child, childInfo);

  OmCacheManager::TouchFreshness();

  return edgeThatGotBroken;
}

std::pair<bool, om::segment::UserEdge> OmSegmentsImpl::JoinFromUserAction(
    const om::segment::UserEdge& e) {
  if (!e.valid) {
    return std::make_pair(false, om::segment::UserEdge());
  }

  std::pair<bool, om::segment::UserEdge> edge = JoinEdgeFromUser(e);
  if (edge.first) {
    userEdges_.push_back(edge.second);
  }
  return edge;
}

std::pair<bool, om::segment::UserEdge> OmSegmentsImpl::JoinEdgeFromUser(
    const om::segment::UserEdge& e) {
  const om::common::SegID childRootID = graph_->Root(e.childID);
  OmSegment* childRoot = store_->GetSegment(childRootID);
  OmSegment* parent = store_->GetSegment(e.parentID);
  OmSegment* parentRoot = FindRoot(parent);

  if (childRoot == parentRoot) {
    log_debug("cycle found in user manual edge; skipping edge %d, %d, %f",
              e.childID, e.parentID, e.threshold);
    return std::make_pair(false, om::segment::UserEdge());
  }

  if (childRoot->IsValidListType() != parent->IsValidListType()) {
    log_debug(
        "not joining child %d to parent %d: child "
        "immutability is %d, but parent's is %d",
        childRoot->value(), parent->value(), childRoot->IsValidListType(),
        parent->IsValidListType());
    return std::make_pair(false, om::segment::UserEdge());
  }

  graph_->Join(childRootID, e.parentID);

  graph_->SegChildren().AddChild(parent, childRoot);
  childRoot->setParent(parent, e.threshold);
  childRoot->setCustomMergeEdge(e);

  FindRoot(parent)->touchFreshnessForMeshes();

  Selection().RerootSegmentList();

  segmentLists_->UpdateSizeListsFromJoin(FindRoot(parent), childRoot);

  return std::make_pair(
      true, om::segment::UserEdge{parent->value(), childRoot->value(),
                                  e.threshold,     true});
}

std::pair<bool, om::segment::UserEdge> OmSegmentsImpl::JoinFromUserAction(
    const om::common::SegID parentID,
    const om::common::SegID childUnknownDepthID) {
  const double threshold = 2.0f;
  return JoinFromUserAction(
      om::segment::UserEdge{parentID, childUnknownDepthID, threshold, true});
}

om::common::SegIDSet OmSegmentsImpl::JoinTheseSegments(
    const om::common::SegIDSet& segmentList) {
  if (segmentList.size() < 2) {
    return om::common::SegIDSet();
  }

  om::common::SegIDSet set = segmentList;  // Join() could modify list

  om::common::SegIDSet ret;  // segments actually joined

  // The first Segment Id is the parent we join to
  om::common::SegIDSet::const_iterator iter = set.begin();
  const om::common::SegID parentID = *iter;
  ++iter;

  // We then iterate through the Segment Ids and join
  // each one to the parent
  while (iter != set.end()) {
    const om::common::SegID segID = *iter;

    std::pair<bool, om::segment::UserEdge> edge =
        JoinFromUserAction(parentID, segID);

    if (edge.first) {
      ret.insert(segID);
    }

    ++iter;
  }

  OmCacheManager::TouchFreshness();

  if (!ret.empty()) {
    ret.insert(parentID);
  }

  return ret;
}

om::common::SegIDSet OmSegmentsImpl::UnJoinTheseSegments(
    const om::common::SegIDSet& segmentList) {
  if (segmentList.size() < 2) {
    return om::common::SegIDSet();
  }

  om::common::SegIDSet set = segmentList;  // split() could modify list
  om::common::SegIDSet ret;

  // The first Segment Id is the parent we split from
  om::common::SegIDSet::const_iterator iter = set.begin();
  const om::common::SegID parentID = *iter;
  ++iter;

  // We then iterate through the Segment Ids and split
  // each one from the parent
  while (iter != set.end()) {
    const om::common::SegID segID = *iter;

    OmSegment* child = store_->GetSegment(segID);

    boost::optional<std::string> splittableTest = IsSegmentSplittable(child);

    if (!splittableTest) {
      splitChildFromParentNoTest(child);
      ret.insert(segID);

    } else {
      log_debugs << "Split error: " << *splittableTest;
    }

    ++iter;
  }

  OmCacheManager::TouchFreshness();

  if (!ret.empty()) {
    ret.insert(parentID);
  }

  return ret;
}

void OmSegmentsImpl::refreshTree() {
  if (!mst_.size() > 0) {
    log_errors << "no mst found...";
  }

  resetGlobalThreshold();

  segmentLists_->RefreshGUIlists();
  OmCacheManager::TouchFreshness();

  log_debugs << "done refreshing tree";
}

void OmSegmentsImpl::setGlobalThreshold() {
  log_info("setting global threshold to %f...",
           OmProject::Globals().Users().UserSettings().getThreshold());
  log_info("setting size threshold to %f...",
           OmProject::Globals().Users().UserSettings().getSizeThreshold());

  thresholder_->SetGlobalThreshold();
  Selection().Clear();
}

void OmSegmentsImpl::resetGlobalThreshold() {
  log_info("resetting global threshold to %f...",
           OmProject::Globals().Users().UserSettings().getThreshold());
  log_info("resetting size threshold to %f...",
           OmProject::Globals().Users().UserSettings().getSizeThreshold());

  thresholder_->ResetGlobalThreshold();
  selection_->RerootSegmentList();
}

void OmSegmentsImpl::Flush() { store_->Flush(); }

bool OmSegmentsImpl::AreAnySegmentsInValidList(
    const om::common::SegIDSet& ids) {
  for (auto& id : ids) {
    OmSegment* seg = store_->GetSegment(id);

    if (!seg) {
      continue;
    }

    if (seg->IsValidListType()) {
      return true;
    }
  }

  return false;
}

om::segment::Children& OmSegmentsImpl::Children() const {
  // TODO: lock?
  return graph_->SegChildren();
}

std::vector<om::segment::UserEdge> OmSegmentsImpl::CutSegment(OmSegment* seg) {
  std::vector<om::segment::UserEdge> edges;

  if (seg->getParent()) {
    edges.push_back(splitChildFromParentNoTest(seg));

  } else {
    const auto children = graph_->SegChildren().GetChildren(seg);

    edges.reserve(children.size());

    for (OmSegment* seg : children) {
      edges.push_back(splitChildFromParentNoTest(seg));
    }
  }

  return edges;
}

bool OmSegmentsImpl::JoinEdges(
    const std::vector<om::segment::UserEdge>& edges) {
  bool joinedAllEdges = true;

  for (auto& e : edges) {
    std::pair<bool, om::segment::UserEdge> edge = JoinEdgeFromUser(e);

    if (!edge.first) {
      joinedAllEdges = false;
    }
  }

  return joinedAllEdges;
}

// depth-first walk of root segment's descendants; return edge vector to allow
// for undo
std::vector<om::segment::UserEdge> OmSegmentsImpl::Shatter(
    OmSegment* segUnknownRoot) {
  OmSegment* seg = FindRoot(segUnknownRoot);
  std::deque<OmSegment*> segs;

  for (OmSegment* s : graph_->SegChildren().GetChildren(seg)) {
    segs.push_back(s);
  }

  OmSegment* segRet = nullptr;
  std::vector<om::segment::UserEdge> edges;

  while (!segs.empty()) {
    segRet = segs.back();
    segs.pop_back();

    for (OmSegment* seg : graph_->SegChildren().GetChildren(segRet)) {
      segs.push_back(seg);
    }

    edges.push_back(splitChildFromParentNoTest(segRet));
  }

  return edges;
}

std::string OmSegmentsImpl::GetName(om::common::SegID segID) {
  if (segmentCustomNames_.empty()) {
    return "";
  }

  auto it = segmentCustomNames_.find(segID);
  if (segmentCustomNames_.end() == it) {
    return "";  // std::string("segment%1").arg(segID);
  }

  return it->second;
}

void OmSegmentsImpl::SetName(om::common::SegID segID, std::string name) {
  segmentCustomNames_[segID] = name;
}

std::string OmSegmentsImpl::GetNote(om::common::SegID segID) {
  if (segmentNotes_.empty()) {
    return "";
  }

  auto it = segmentNotes_.find(segID);
  if (segmentNotes_.end() == it) {
    return "";
  }

  return it->second;
}

void OmSegmentsImpl::SetNote(om::common::SegID segID, std::string note) {
  segmentNotes_[segID] = note;
}

OmSegment* OmSegmentsImpl::FindRoot(const om::common::SegID segID) {
  auto root = FindRootID(segID);
  if (root) {
    return store_->GetSegment(root);
  } else {
    return nullptr;
  }
}

OmSegment* OmSegmentsImpl::FindRoot(OmSegment* segment) {
  auto root = FindRootID(segment);
  if (root) {
    return store_->GetSegment(root);
  } else {
    return nullptr;
  }
}

bool OmSegmentsImpl::IsSegmentValid(om::common::SegID seg) {
  return store_->IsSegmentValid(seg);
}
OmSegment* OmSegmentsImpl::GetSegment(const om::common::SegID value) const {
  return store_->GetSegment(value);
}
