
#include "datalayer/archive/filter.h"
#include "datalayer/archive/genericManager.hpp"
#include "volume/omFilter2dManager.h"
#include "volume/omFilter2d.h"

namespace om {
namespace data {
namespace archive {


YAML::Emitter &operator<<(YAML::Emitter& out, const OmFilter2dManager& fm)
{
    out << YAML::BeginMap;
    genericManager::Save(out, fm.filters_);
    out << YAML::EndMap;
    return out;
}

void operator>>(const YAML::Node& in, OmFilter2dManager& fm)
{
    genericManager::Load(in, fm.filters_);
}

YAML::Emitter &operator<<(YAML::Emitter& out, const OmFilter2d& f)
{
    out << YAML::BeginMap;
    out << YAML::Key << "id" << YAML::Value << f.GetID();
    out << YAML::Key << "note" << YAML::Value << f.GetNote().toStdString();
    out << YAML::Key << "custom name" << YAML::Value << f.GetCustomName().toStdString();
    out << YAML::Key << "alpha" << YAML::Value << f.alpha_;
    out << YAML::Key << "chan id" << YAML::Value << f.chanID_;
    out << YAML::Key << "seg id" << YAML::Value << f.segID_;
    out << YAML::EndMap;
    
    return out;
}

void operator>>(const YAML::Node& in, OmFilter2d& f)
{
    in["id"] >> f.id_;
    in["note"] >> f.note_;
    in["custom name"] >> f.customName_;
    in["alpha"] >> f.alpha_;
    in["chan id"] >> f.chanID_;
    in["seg id"] >> f.segID_;
}

}; // namespace archive
}; // namespace data
}; // namespace om