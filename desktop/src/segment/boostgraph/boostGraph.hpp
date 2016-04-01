#include "precomp.h"

#include "utility/boost/graph/types.hpp"
#include "segment/omSegments.h"
#include "segment/lowLevel/children.hpp"

using namespace om::segment::boostgraph;

class BoostGraphFactory {
 public:
   BoostGraphFactory(const Children& children);
   BoostGraph Get(OmSegment* rootSegment);
 private:
   const Children& children_;
}

class BoostGraph {
  Color SOURCE = DefaultColors::black();
  Color SINK = DefaultColors::white();

 public:
  BoostGraph(const Children& children, OmSegment* rootSeg);

  static Graph GetGraph(OmSegment* rootSeg);
  static std::unordered_map<om::common::SegID, Vertex> GetVertexMapping(Graph graph);

  std::vector<om::segment::Edge> MinCut(
      om::common::SegIDSet sources, om::common::SegIDSet sinks);

  om::segment::Edge ToSegmentEdge(Edge);
  std::vector<Edge> GetMinCutEdges();

 private:
  Children& children_;
  Graph graph_;
  std::unordered_map<om::common::SegID, Vertex> idToVertex_;

  CapacityProperty capacityProperty_;
  ColorProperty colorProperty_;
  NameProperty nameProperty_;
  ResidualCapacityProperty residualCapacityProperty_;
  ReverseProperty reverseProperty_;
  

  // Graph creation helpers
  void addEdge(Vertex& vertex1, Vertex& vertex2, int threshold);
  void addVertex(OmSegment* segment);
};
