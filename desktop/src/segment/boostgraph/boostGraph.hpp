#pragma once
#include "precomp.h"

#include "segment/boostgraph/types.hpp"
#include "segment/lowLevel/children.hpp"
#include <sstream>
#include <string>

using namespace om::segment::boostgraph;

class BoostGraph {
 public:
  static const double HARD_LINK_WEIGHT;
  static const Color COLOR_SOURCE;
  static const Color COLOR_SINK;
  static const Color COLOR_FREE;

  BoostGraph(const om::segment::Children& children);
  BoostGraph(const om::segment::Children& children, const OmSegment* rootSeg);

  void SetGraph(Graph graph);
  Graph& GetGraph();
  Vertex& GetVertex(om::common::SegID segID);

  virtual std::vector<om::segment::Edge> MinCut(
      const om::common::SegIDSet sources, const om::common::SegIDSet sinks);

  std::tuple<Vertex, Vertex> MakeSingleSourceSink(
      const om::common::SegIDSet sources, const om::common::SegIDSet sinks);

  om::segment::Edge ToSegmentEdge(const Edge edge);
  std::vector<Edge> GetMinCutEdges(Vertex vertex);

  void BuildGraph(const OmSegment* rootSeg);

  void Print();
 private:
  const om::segment::Children& children_;
  Graph graph_;
  std::unordered_map<om::common::SegID, Vertex> idToVertex_;

  NameProperty nameProperty_;
  CapacityProperty capacityProperty_;
  ReverseProperty reverseProperty_;
  ColorProperty colorProperty_;
  SegmentIDProperty segmentIDProperty_;

  void setProperties();
  // Graph creation helper functions
  void buildGraphDfsVisit(Vertex parentVertex);
  Vertex addVertex(const OmSegment* segment);
  std::tuple<Edge, Edge, bool> addEdge(Vertex& vertex1,
      Vertex& vertex2, double threshold);

  // attach all vertices with these ids to a common vertex
  // isSource = true, means we flow into selected segIds
  Vertex createCommonVertex(om::common::SegIDSet ids, bool isSource);

  // helper functions to print out what we are cutting easier
  std::string idSetToStringStream(om::common::SegIDSet segIDs);
  std::string edgesToStringStream(std::vector<om::segment::Edge> edges);
};

class BoostGraphFactory {
 public:
   BoostGraphFactory(const om::segment::Children& children);
   virtual std::shared_ptr<BoostGraph> Get(const OmSegment* rootSegment) const;
 private:
   const om::segment::Children& children_;
};
