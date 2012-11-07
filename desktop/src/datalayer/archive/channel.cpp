#include "yaml-cpp/yaml.h"
#include "utility/yaml/omYaml.hpp"
#include "project/details/omChannelManager.h"
#include "utility/yaml/mipVolume.hpp"
#include "utility/yaml/baseTypes.hpp"
#include "datalayer/archive/filter.h"
#include "utility/yaml/genericManager.hpp"

namespace YAML {

Emitter &operator<<(Emitter& out, const OmChannelManager& cm)
{
    out << BeginMap;
    GenericManager::Save(out, cm.manager_);
    out << EndMap;
    return out;
}

void operator>>(const Node& in, OmChannelManager& cm)
{
    GenericManager::Load(in, cm.manager_);
}

Emitter& operator<<(Emitter& out, const OmChannel& chan)
{
    out << BeginMap;
    mipVolume<const OmChannel> volArchive(chan);
    volArchive.Store(out);

    out << Key << "Filters" << Value << chan.filterManager_;
    out << EndMap;
    return out;
}

void operator>>(const Node& in, OmChannel& chan)
{
    mipVolume<OmChannel> volArchive(chan);
    volArchive.Load(in);

    in["Filters"] >> chan.filterManager_;
    chan.LoadVolDataIfFoldersExist();
}

} // namespace YAML
