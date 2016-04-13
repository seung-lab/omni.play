#pragma once
#include <boost/graph/graphviz.hpp>

template <class Prop>
class label_writer {
 public:
  label_writer(Prop& prop) : prop_(prop) {}
  template <class VertexOrEdge>
  void operator()(std::ostream& out, const VertexOrEdge& v) const {
    out << "[label=\"" << prop_[v] << "\"]";
  }
private:
  Prop& prop_;
};

template <class Prop1, class Prop2>
class label_writer_2 {
 public:
  label_writer_2(Prop1& prop1, Prop2& prop2) : prop1_(prop1), prop2_(prop2) {}
  template <class VertexOrEdge>
  void operator()(std::ostream& out, const VertexOrEdge& v) const {
    out << "[label=\"" << prop1_[v] << ", "  << prop2_[v] << "\"]";
  }
private:
  Prop1& prop1_;
  Prop2& prop2_;
};

template <class Prop1, class Prop2>
inline label_writer_2<Prop1, Prop2>
make_label_writer_2(Prop1 prop1, Prop2 prop2) {
  return label_writer_2<Prop1, Prop2>(prop1, prop2);
};

