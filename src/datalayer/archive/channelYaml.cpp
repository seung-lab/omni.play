#include "yaml-cpp/yaml.h"
#include "utility/yaml/yaml.hpp"
#include "project/details/channelManager.h"
#include "datalayer/archive/volume.hpp"
#include "utility/yaml/baseTypes.hpp"
#include "datalayer/archive/filter.h"
#include "utility/yaml/genericManager.hpp"

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

Emitter& operator<<(Emitter& out, const coords::volumeSystem& c)
{
    out << BeginMap;
    // out << Key << "dataDimensions" << Value << c.GetDataDimensions();
    // out << Key << "dataResolution" << Value << c.GetResolution();
    // out << Key << "chunkDim" << Value << c.chunkDim_;
    // out << Key << "mMipLeafDim" << Value << c.mMipLeafDim;
    // out << Key << "mMipRootLevel" << Value << c.mMipRootLevel;
    // out << Key << "absOffset" << Value << c.GetAbsOffset();
    out << EndMap;
    return out;
}

void operator>>(const Node& in, coords::volumeSystem& c)
{
    boost::optional<coords::globalBbox> extent;
    yaml::yamlUtil::OptionalRead(in, "dataExtent", extent); // backwards compatibility
    if(extent) {
        c.SetDataDimensions(extent.get().getDimensions());
    } else {
        Vector3i dims;
        in["dataDimensions"] >> dims;
        c.SetDataDimensions(dims);
    }

    Vector3i resolution;
    in["dataResolution"] >> resolution;
    c.SetResolution(resolution);

    int chunkDim;
    in["chunkDim"] >> chunkDim;
    c.SetChunkDimension(chunkDim);

    c.UpdateRootLevel();
    Vector3i offset;
    yaml::yamlUtil::OptionalRead(in, "absOffset", offset, Vector3i::ZERO);
    c.SetAbsOffset(offset);
}

} // namespace YAML
