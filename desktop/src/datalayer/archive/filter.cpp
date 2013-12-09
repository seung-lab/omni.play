
#include "datalayer/archive/filter.h"
#include "volume/omFilter2dManager.h"
#include "volume/omFilter2d.h"
#include "utility/yaml/genericManager.hpp"

namespace YAMLold {

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

}  // namespace YAMLold
