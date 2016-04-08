#pragma once
#include "precomp.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include "segment/omSegment.h"

namespace om {
namespace segment {
namespace boostgraph {


struct vertex_segment_t {
  typedef boost::vertex_property_tag kind;
};


typedef typename boost::color_traits<boost::default_color_type> DefaultColors;
typedef typename boost::default_color_type Color;
typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> Traits;
typedef typename Traits::vertex_descriptor Vertex;
typedef typename Traits::edge_descriptor Edge;
typedef typename boost::property<boost::vertex_name_t, std::string,
          boost::property<boost::vertex_index_t, long,
          boost::property<boost::vertex_color_t, boost::default_color_type,
          boost::property<boost::vertex_distance_t, long,
          boost::property<boost::vertex_predecessor_t, Edge,
          boost::property<vertex_segment_t,const OmSegment*>>>>>> VertexProperties;

typedef typename boost::property<boost::edge_capacity_t, long,
          boost::property<boost::edge_residual_capacity_t, long,
          boost::property<boost::edge_reverse_t, Edge>>> EdgeProperties;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
        VertexProperties, EdgeProperties> Graph;
typedef boost::template property_map<Graph,
        boost::edge_capacity_t>::type CapacityProperty;
typedef boost::template property_map<Graph,
        boost::edge_residual_capacity_t>::type ResidualCapacityProperty;
typedef boost::template property_map<Graph,
        boost::edge_reverse_t>::type ReverseProperty;
typedef boost::template property_map<Graph,
        boost::vertex_name_t>::type NameProperty;
typedef boost::template property_map<Graph,
        boost::vertex_color_t>::type ColorProperty;
typedef boost::template property_map<Graph,
        vertex_segment_t>::type SegmentProperty;

} // namespace boostgraph
} // namespace segment
} // namespace om
