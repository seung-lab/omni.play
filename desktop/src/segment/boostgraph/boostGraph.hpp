#pragma once
#include "precomp.h"

#include "segment/boostgraph/types.hpp"
#include "segment/lowLevel/children.hpp"

using namespace om::segment::boostgraph;

class BoostGraph {
  Color SOURCE = DefaultColors::black();
  Color SINK = DefaultColors::white();

 public:
  BoostGraph(const om::segment::Children& children);
  BoostGraph(const om::segment::Children& children, const OmSegment* rootSeg);

  Graph GetGraph();
  void SetGraph(Graph graph);

  virtual std::vector<om::segment::UserEdge> MinCut(
      const om::common::SegIDSet sources, const om::common::SegIDSet sinks);

  std::tuple<Vertex, Vertex> MakeSingleSourceSink(
      const om::common::SegIDSet sources, const om::common::SegIDSet sinks);

  static om::segment::UserEdge ToSegmentUserEdge(const Edge edge);
  std::vector<Edge> GetMinCutEdges();

  Graph BuildGraph(const OmSegment* rootSeg);

 private:
  const om::segment::Children& children_;
  Graph graph_;
  std::unordered_map<om::common::SegID, Vertex> idToVertex_;

  NameProperty nameProperty_;
  CapacityProperty capacityProperty_;
  ReverseProperty reverseProperty_;
  ColorProperty colorProperty_;
  //ResidualCapacityProperty residualCapacityProperty_;

  // Graph creation helper functions
  void buildGraphDfsVisit(const OmSegment* segment);
  Vertex addVertex(const OmSegment* segment);
  void addEdge(Vertex& vertex1, Vertex& vertex2, int threshold);
};

class BoostGraphFactory {
 public:
   BoostGraphFactory(const om::segment::Children& children);
   virtual std::shared_ptr<BoostGraph> Get(const OmSegment* rootSegment) const;
 private:
   const om::segment::Children& children_;
};

