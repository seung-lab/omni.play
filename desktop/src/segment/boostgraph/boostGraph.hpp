#pragma once
#include "precomp.h"

#include "segment/boostgraph/types.hpp"
#include "segment/lowLevel/children.hpp"

using namespace om::segment::boostgraph;

class BoostGraph {
 public:
  static const double HARD_LINK_WEIGHT;
  static const Color COLOR_SOURCE;
  static const Color COLOR_SINK;


  BoostGraph(const om::segment::Children& children);
  BoostGraph(const om::segment::Children& children, const OmSegment* rootSeg);

  void SetGraph(Graph graph);
  Graph& GetGraph();
  Vertex& GetVertex(om::common::SegID segID);

  virtual std::vector<om::segment::UserEdge> MinCut(
      const om::common::SegIDSet sources, const om::common::SegIDSet sinks);

  std::tuple<Vertex, Vertex> MakeSingleSourceSink(
      const om::common::SegIDSet sources, const om::common::SegIDSet sinks);

  static om::segment::UserEdge ToSegmentUserEdge(const Edge edge);
  std::vector<Edge> GetMinCutEdges();

  void BuildGraph(const OmSegment* rootSeg);

 private:
  const om::segment::Children& children_;
  Graph graph_;
  std::unordered_map<om::common::SegID, Vertex> idToVertex_;

  NameProperty nameProperty_;
  CapacityProperty capacityProperty_;
  ReverseProperty reverseProperty_;
  ColorProperty colorProperty_;
  SegmentIDProperty segmentIDProperty_;
  //ResidualCapacityProperty residualCapacityProperty_;

  void setProperties();
  // Graph creation helper functions
  void buildGraphDfsVisit(Vertex parentVertex);
  Vertex addVertex(const OmSegment* segment);
  void addEdge(Vertex& vertex1, Vertex& vertex2, double threshold);
};

class BoostGraphFactory {
 public:
   BoostGraphFactory(const om::segment::Children& children);
   virtual std::shared_ptr<BoostGraph> Get(const OmSegment* rootSegment) const;
 private:
   const om::segment::Children& children_;
};

