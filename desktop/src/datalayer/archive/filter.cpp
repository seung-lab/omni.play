#include "datalayer/archive/filter.h"
#include "utility/yaml/genericManager.hpp"
#include "volume/omFilter2dManager.h"
#include "volume/omFilter2d.h"

namespace YAML {
Node convert<OmFilter2dManager>::encode(const OmFilter2dManager& fm) {
  return genericManager::Save(fm.filters_);
}
bool convert<OmFilter2dManager>::decode(const Node& node,
                                        OmFilter2dManager& fm) {
  return genericManager::Load(node, fm.filters_);
}

Node convert<OmFilter2d>::encode(const OmFilter2d& f) {
  Node n;
  n["id"] = f.GetID();
  n["note"] = f.GetNote().toStdString();
  n["custom name"] = f.GetCustomName().toStdString();
  n["alpha"] = f.alpha_;
  n["chan id"] = f.chanID_;
  n["seg id"] = f.segID_;
  return n;
}
bool convert<OmFilter2d>::decode(const Node& node, OmFilter2d& f) {
  if (!node.IsMap()) {
    return false;
  }
  try {
    f.id_ = node["id"].as<om::common::ID>(0);
    f.note_ = node["note"].as<QString>("");
    f.customName_ = node["custom name"].as<QString>("");
    f.alpha_ = node["alpha"].as<double>(0);
    f.chanID_ = node["chan id"].as<om::common::ID>(0);
    f.segID_ = node["seg id"].as<om::common::ID>(0);
  }
  catch (YAML::Exception e) {
    log_debugs << "Error Decoding OmFilter2d: " << e.what();
    return false;
  }
  return true;
}

}  // namespace YAML
