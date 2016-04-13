#pragma once
#include "precomp.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "segment/omSegment.h"
#include "segment/boostgraph/boostGraph.hpp"
#include "segment/boostgraph/types.hpp"

namespace test {
namespace boostgraph {
/*
 * Makes sure that the given element exists and it's connect to the correct
 * parent with the correct weight
 */
void validateSegmentBuilt(OmSegment* segment, Graph graph);

/*
 * Verify that the input new source and new sink is correctly
 * connected to all the specified sources and sinks
 */
void verifySinkSource(Vertex newSourceVertex, Vertex newSinkVertex,
    om::common::SegIDSet sources, om::common::SegIDSet sinks,
    BoostGraph boostGraph);

/*
 * This is a templated helper function that helps compare the expected edges
 * of type U_EXP vs the actual edges of type V_ACT.
 * Parameters edgeToSource and edgeToSink is an adapter function that 
 * converts from V_ACT to U_EXP for the comparison
 */
template <typename U_EXP, typename V_ACT>
void validateCutEdges(std::vector<std::tuple<U_EXP, U_EXP>>& expectedEdges,
    std::vector<V_ACT>& actualEdges, Graph& graph,
    std::function<U_EXP(V_ACT)> edgeToSourceFunction,
    std::function<U_EXP(V_ACT)> edgeToTargetFunction);

/*
 * Convert from Graph::Edge to source Graph::Vertex
 */
std::tuple<std::function<Vertex(Edge)>, std::function<Vertex(Edge)>>
  getEdgeToVertexFunctions(Graph& graph);

/*
 * Convert from om::segment::UserEdge to source om::common::SegID
 */
std::tuple<std::function<om::common::SegID(om::segment::UserEdge)>,
  std::function<om::common::SegID(om::segment::UserEdge)>>
  getUserEdgeToSegIDFunctions();

} // namespace boostgraph
} // namespace test

