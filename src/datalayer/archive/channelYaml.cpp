#include "yaml-cpp/yaml.h"
#include "utility/yaml/yaml.hpp"
#include "project/details/channelManager.h"
#include "datalayer/archive/volume.hpp"
#include "utility/yaml/baseTypes.hpp"
#include "datalayer/archive/filter.h"
#include "utility/yaml/genericManager.hpp"
#include "coordinates/yaml.h"

using namespace om;

namespace YAML {

Emitter &operator<<(Emitter& out, const proj::channelManager& cm)
{
    out << BeginMap;
    genericManager::Save(out, cm.manager_);
    out << EndMap;
    return out;
}

void operator>>(const Node& in, proj::channelManager& cm)
{
    genericManager::Load(in, cm.manager_);
}

Emitter& operator<<(Emitter& out, const om::volume::channel& chan)
{
    out << BeginMap;
    YAML::volume<const om::volume::channel> volArchive(chan);
    volArchive.Store(out);
    out << EndMap;
    return out;
}

void operator>>(const Node& in, om::volume::channel& chan)
{
    YAML::volume<om::volume::channel> volArchive(chan);
    volArchive.Load(in);

    chan.LoadVolDataIfFoldersExist();
}

} // namespace YAML
