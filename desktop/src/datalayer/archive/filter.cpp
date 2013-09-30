
#include "datalayer/archive/filter.h"
#include "volume/omFilter2dManager.h"
#include "volume/omFilter2d.h"
#include "utility/yaml/genericManager.hpp"

namespace YAML {
void operator>>(const YAML::Node& in, uint32_t& p) { p = in.as<uint32_t>(); }
inline YAML::Emitter& operator<<(YAML::Emitter& out, const QString& s) {
  return out << s.toStdString();
}

inline void operator>>(const YAML::Node& in, QString& s) {
  std::string str = in.as<std::string>();

  if (str == "~")  // NULL Value from YAML
    str = "";

  s = QString::fromStdString(str);
}

void operator>>(const YAML::Node& in, double& p) { p = in.as<double>(); }
template <class T>
YAML::Emitter& operator<<(YAML::Emitter& out, const std::unordered_set<T>& s) {
  out << YAML::Flow << YAML::BeginSeq;
  for (const auto& e : s) {
    out << e;
  }
  out << YAML::EndSeq;
  return out;
}

template <class T>
    void operator>>(const YAML::Node& in, std::unordered_set<T>& s) {
  FOR_EACH(it, in) {
    T item;
    *it >> item;
    s.insert(item);
  }
}

Emitter& operator<<(Emitter& out, const OmFilter2dManager& fm) {
  out << BeginMap;
  genericManager::Save(out, fm.filters_);
  out << EndMap;
  return out;
}

void operator>>(const Node& in, OmFilter2dManager& fm) {
  genericManager::Load(in, fm.filters_);
}

Emitter& operator<<(Emitter& out, const OmFilter2d& f) {
  out << BeginMap;
  out << Key << "id" << Value << f.GetID();
  out << Key << "note" << Value << f.GetNote().toStdString();
  out << Key << "custom name" << Value << f.GetCustomName().toStdString();
  out << Key << "alpha" << Value << f.alpha_;
  out << Key << "chan id" << Value << f.chanID_;
  out << Key << "seg id" << Value << f.segID_;
  out << EndMap;

  return out;
}

void operator>>(const Node& in, OmFilter2d& f) {
  in["id"] >> f.id_;
  in["note"] >> f.note_;
  in["custom name"] >> f.customName_;
  in["alpha"] >> f.alpha_;
  in["chan id"] >> f.chanID_;
  in["seg id"] >> f.segID_;
}

}  // namespace YAML
