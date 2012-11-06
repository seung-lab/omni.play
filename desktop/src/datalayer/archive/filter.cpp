
#include "datalayer/archive/filter.h"
#include "volume/omFilter2dManager.h"
#include "volume/omFilter2d.h"
#include "utility/yaml/genericManager.hpp"

namespace YAML {

Emitter &operator<<(Emitter& out, const OmFilter2dManager& fm)
{
    out << BeginMap;
    GenericManager::Save(out, fm.filters_);
    out << EndMap;
    return out;
}

void operator>>(const Node& in, OmFilter2dManager& fm)
{
    GenericManager::Load(in, fm.filters_);
}

Emitter &operator<<(Emitter& out, const OmFilter2d& f)
{
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

void operator>>(const Node& in, OmFilter2d& f)
{
    in["id"] >> f.id_;
    std::string note;
    in["note"] >> note; f.note_ = QString::fromStdString(note);
    std::string customName;
    in["custom name"] >> customName; f.customName_ = QString::fromStdString(customName);
    in["alpha"] >> f.alpha_;
    in["chan id"] >> f.chanID_;
    in["seg id"] >> f.segID_;
}

} // namespace YAML