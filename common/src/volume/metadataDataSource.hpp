#pragma once

#include "datalayer/dataSource.hpp"
#include "volume/metadata.hpp"
#include "coordinates/yaml.hpp"

#include "utility/yaml/baseTypes.hpp"
#include "utility/yaml/yaml.hpp"
#include <boost/filesystem.hpp>

#include <boost/regex.hpp>
#include <string>

namespace om {
namespace volume {

class MetadataDataSource
    : public datalayer::IDataSource<std::string, Metadata> {
 public:
  static std::shared_ptr<Metadata> GetStatic(const std::string& uri) {
    MetadataDataSource mds;
    return mds.Get(uri);
  }
  virtual std::shared_ptr<Metadata> Get(const std::string& uri,
                                        bool async = false) override {
    std::string path;
    common::ObjectType volType;
    int number;
    try {
      if (parseUri(uri, path, volType, number)) {
        log_debugs << "Loading Metadata " << path << " : " << number;
        return loadYaml(path, volType, number);
      } else {
        return std::shared_ptr<Metadata>();
      }
    }
    catch (YAML::Exception e) {
      log_debugs << "Failed Loading Metadata: " << e.what();
      return std::shared_ptr<Metadata>();
    }
    catch (Exception e) {
      log_debugs << "Failed Loading Metadata: " << e.what();
      return std::shared_ptr<Metadata>();
    }
  }

  virtual bool Put(const std::string& uri, std::shared_ptr<Metadata> metadata,
                   bool asnyc = false) override {
    std::string path;
    common::ObjectType volType;
    int number;
    try {
      if (parseUri(uri, path, volType, number)) {
        log_debugs << "Writing Metadata " << path << " : " << number;
        return writeYaml(path, volType, number, metadata);
      } else {
        return false;
      }
    }
    catch (YAML::Exception e) {
      log_debugs << "Failed Writing Metadata: " << e.what();
      return false;
    }
    catch (Exception e) {
      log_debugs << "Failed Writing Metadata: " << e.what();
      return false;
    }
  }

  virtual bool is_cache() const override { return false; }
  virtual bool is_persisted() const override { return true; }

 private:
  bool parseUri(const std::string& uri, std::string& path,
                common::ObjectType& volType, int& number) const {
    if (!boost::filesystem::exists(uri) ||
        !boost::filesystem::is_directory(uri)) {
      return false;
    }
    boost::regex r(
        "(.*)/(segmentations|channels)/(segmentation|channel)(\\d+)/?");

    boost::match_results<std::string::const_iterator> mr;
    if (!boost::regex_match(uri, mr, r)) {
      throw IoException("Invalid volume URI: " + uri);
    }

    path = mr[1].str() + "/projectMetadata.yaml";
    if (mr[2] == "channels") {
      volType = common::ObjectType::CHANNEL;
    } else {
      volType = common::ObjectType::SEGMENTATION;
    }

    number = std::stoi(mr[4]);
    return true;
  }

  bool writeYaml(const std::string& path, const common::ObjectType volType,
                 int number, std::shared_ptr<Metadata> metadata) {
    auto docs = YAML::LoadAllFromFile(path);

    YAML::Node volume =
        volType == common::ObjectType::CHANNEL
            ? docs[1]["Volumes"]["Channels"]["values"][number - 1]
            : docs[1]["Volumes"]["Segmentations"]["values"][number - 1];

    if (volType == common::ObjectType::SEGMENTATION) {
      volume["Segments"]["Num Segments"] = metadata->NumSegments;
      volume["Segments"]["Max Value"] = metadata->MaxSegments;
      volume["Num Edges"] = metadata->NumEdges;
    }

    volume["custom name"] = metadata->Name;
    volume["UUID"] = metadata->UUID.Str();
    volume["type"] = metadata->DataType.value();

    volume["coords"]["dataDimensions"] = metadata->Bounds.getDimensions();
    volume["coords"]["absOffset"] = metadata->Bounds.getMin();
    volume["coords"]["dataResolution"] = metadata->Resolution;
    volume["coords"]["chunkDim"] = metadata->ChunkDim;
    volume["coords"]["mMipRootLevel"] = metadata->RootMipLevel;

    if (volType == common::ObjectType::CHANNEL) {
      docs[1]["Volumes"]["Channels"]["values"][number - 1] = volume;
    } else {
      docs[1]["Volumes"]["Segmentations"]["values"][number - 1] = volume;
    }

    std::ofstream fout(path.c_str());
    YAML::Emitter e(fout);
    e << YAML::BeginDoc << docs[0] << YAML::EndDoc;
    e << YAML::BeginDoc << docs[1] << YAML::EndDoc;

    return true;
  }

  std::shared_ptr<Metadata> loadYaml(const std::string& path,
                                     const common::ObjectType volType,
                                     int number) {
    auto docs = YAML::LoadAllFromFile(path);

    std::shared_ptr<Metadata> ret(new Metadata());
    ret->VolumeType = volType;

    YAML::Node volume;
    std::string defaultName;
    if (volType == common::ObjectType::CHANNEL) {
      volume = docs[1]["Volumes"]["Channels"]["values"][number - 1];
      defaultName = "channel" + std::to_string(number);
    } else {
      volume = docs[1]["Volumes"]["Segmentations"]["values"][number - 1];
      defaultName = "segmentation" + std::to_string(number);
      ret->NumSegments = volume["Segments"]["Num Segments"].as<int>();
      ret->MaxSegments = volume["Segments"]["Max Value"].as<int>();
      ret->NumEdges = volume["Num Edges"].as<int>();
    }
    ret->Name = volume["custom name"].as<std::string>(defaultName);

    if (volume["UUID"].IsDefined()) {
      ret->UUID = utility::UUID(volume["UUID"].as<std::string>());
    } else {
      ret->UUID = utility::UUID();
    }

    boost::optional<om::common::DataType> dt =
        om::common::DataType::get_by_value(volume["type"].as<std::string>());

    if (dt) {
      ret->DataType = *dt;
    } else {
      throw IoException("Invalid Data type.");
    }

    coords::Global dims;
    if (volume["coords"]["dataDimensions"].IsDefined()) {
      dims = volume["coords"]["dataDimensions"].as<coords::Global>();
    } else {
      dims = volume["coords"]["dataExtent"]["max"].as<coords::Global>() -
             volume["coords"]["dataExtent"]["min"].as<coords::Global>();
    }
    auto absOffset =
        volume["coords"]["absOffset"].as<coords::Global>(coords::Global::ZERO);
    ret->Resolution =
        volume["coords"]["dataResolution"].as<Vector3i>(Vector3i::ONE);
    ret->ChunkDim = volume["coords"]["chunkDim"].as<int>(128);
    ret->RootMipLevel = volume["coords"]["mMipRootLevel"].as<int>();

    dims.x = om::math::roundUp((int)dims.x, ret->ChunkDim);
    dims.y = om::math::roundUp((int)dims.y, ret->ChunkDim);
    dims.z = om::math::roundUp((int)dims.z, ret->ChunkDim);

    ret->Bounds = coords::GlobalBbox(absOffset, absOffset + dims);

    return ret;
  }
};
}
}  // namespace om::volume::