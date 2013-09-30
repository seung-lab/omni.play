#pragma once

#include "datalayer/file.h"
#include "common/enums.hpp"
#include "coordinates/chunk.h"
#include "utility/UUID.hpp"
#include <string>

namespace om {
namespace file {

class Paths {
 public:
  Paths(const path& omniFile) : omniFile_(om::file::absolute(omniFile)) {
    // TODO: More sophisticated handling (make sure it ends .omni and .files
    // exists)
    filesFolder_ = omniFile_;
    filesFolder_ += ".files";
  }

  path OmniFile() const { return omniFile_; }
  path FilesFolder() const { return filesFolder_; }

  // Filenames
  struct File {
    static path ProjectMetadataQt() { return "projectMetadata.qt"; }
    static path ProjectMetadataYaml() { return "projectMetadata.yaml"; }
    static path RandColors() { return "rand_colors.raw.ver1"; }
    static path Locations() { return "locations"; }
    static path UniqueValues() { return "uniqeValues.1.0000.ver1"; }
    static path MeshAllocTable() { return "meshAllocTable.ver2"; }
    static path MeshData() { return "meshData.ver2"; }
    static path MST() { return "mst.data"; }
    static path UserEdges() { return "mstUserEdges.data"; }
    static path SegmentDataPage(uint8_t page) {
      return std::string("segment_page") + std::to_string(page) + ".data.ver4";
    }
    static path SegmentListTypesPage(uint8_t page) {
      return std::string("segment_page") + std::to_string(page) +
             ".list_types.ver4";
    }
    static path ValidGroupNum() { return "valid_group_num.data.ver1"; }
    static path Settings() { return "settings.yaml"; }
    static path Annotations() { return "annotations.yml"; }
    static path Volume(const common::DataType& type) {
      return std::string("volume.") + type.value() + ".raw";
    }
    static path LongRangeConnections() { return "LongRangeConnections.txt"; }
  };

  // Relative to FilesFolder
  struct Root {
    static path Channels() { return "channels"; }
    static path Channel(uint8_t i) {
      return Channels() / ("channel" + std::to_string(i));
    }
    static path Segmentations() { return "segmentations"; }
    static path Segmentation(uint8_t i) {
      return Segmentations() / ("segmentation" + std::to_string(i));
    }
    static path Users() { return "users"; }
    static path User(std::string username) { return Users() / username; }
  };

  // Relative to Volume
  struct Vol {
    static path Data(uint8_t mipLevel, const common::DataType& type) {
      return Mip(mipLevel) / File::Volume(type);
    }
  };

  // Relative to Segmentation
  struct Seg {
    static path Chunks() { return "chunks"; }
    static path Chunk(const coords::Chunk& c) {
      return Chunks() / Paths::Chunk(c);
    }
    static path ChunkUniqueValues(const coords::Chunk& c) {
      return Chunk(c) / File::UniqueValues();
    }
    static path Meshes() { return "meshes/1.0000"; }
    static path MeshAllocTable(const coords::Chunk& c) {
      return Meshes() / Paths::Chunk(c) / File::MeshAllocTable();
    }
    static path MeshData(const coords::Chunk& c) {
      return Meshes() / Paths::Chunk(c) / File::MeshData();
    }
    static path Segments() { return "segments"; }
    static path SegmentDataPage(uint8_t page) {
      return Segments() / File::SegmentDataPage(page);
    }
    static path SegmentListTypesPage(uint8_t page) {
      return Segments() / File::SegmentListTypesPage(page);
    }
    static path ValidGroupNum() { return Segments() / File::ValidGroupNum(); }
    static path MST() { return Segments() / File::MST(); }
    static path UserEdges() { return Segments() / File::UserEdges(); }
  };

  struct Usr {
    static path LogFiles() { return "logFiles"; }
  };

  // Relative
  static path Mip(uint8_t i) { return std::to_string(i); }
  static path Chunk(const coords::Chunk& c) {
    return toPath(c.mipLevel()) / toPath(c.x) / toPath(c.y) / toPath(c.z);
  }

  // Absolute
  path ProjectMetadataQt() const {
    return FilesFolder() / File::ProjectMetadataQt();
  }
  path ProjectMetadataYaml() const {
    return FilesFolder() / File::ProjectMetadataYaml();
  }
  path RandColors() const { return FilesFolder() / File::RandColors(); }
  path Locations() const { return FilesFolder() / File::Locations(); }

  path Channels() const { return FilesFolder() / Root::Channels(); }
  path Channel(uint8_t i) const { return FilesFolder() / Root::Channel(i); }
  path ChannelData(uint8_t i, uint8_t mipLevel, const common::DataType& type) {
    return Channel(i) / Vol::Data(mipLevel, type);
  }

  path Segmentations() const { return FilesFolder() / Root::Segmentations(); }
  path Segmentation(uint8_t i) const {
    return FilesFolder() / Root::Segmentation(i);
  }
  path SegmentationData(uint8_t i, uint8_t mipLevel,
                        const common::DataType& type) {
    return Segmentation(i) / Vol::Data(mipLevel, type);
  }

  path SegChunks(uint8_t i) const { return Segmentation(i) / Seg::Chunks(); }
  path SegChunk(uint8_t i, const coords::Chunk& c) const {
    return Segmentation(i) / Seg::Chunk(c);
  }
  path ChunkUniqueValues(uint8_t i, const coords::Chunk& c) const {
    return Segmentation(i) / Seg::ChunkUniqueValues(c);
  }

  path Meshes(uint8_t i) const { return Segmentation(i) / Seg::Meshes(); }
  path MeshAllocTable(uint8_t i, const coords::Chunk& c) const {
    return Segmentation(i) / Seg::MeshAllocTable(c);
  }
  path MeshData(uint8_t i, const coords::Chunk& c) const {
    return Segmentation(i) / Seg::MeshData(c);
  }

  path Segments(uint8_t i) const { return Segmentation(i) / Seg::Segments(); }
  path SegmentDataPage(uint8_t i, uint8_t page) const {
    return Segmentation(i) / Seg::SegmentDataPage(page);
  }
  path SegmentListTypesPage(uint8_t i, uint8_t page) const {
    return Segmentation(i) / Seg::SegmentListTypesPage(page);
  }
  path ValidGroupNum(uint8_t i) const {
    return Segmentation(i) / Seg::ValidGroupNum();
  }
  path MST(uint8_t i) const { return Segmentation(i) / Seg::MST(); }
  path UserEdges(uint8_t i) const { return Segmentation(i) / Seg::UserEdges(); }

  // Users
  path Users() const { return FilesFolder() / Root::Users(); }
  path User(std::string username) const {
    return FilesFolder() / Root::User(username);
  }
  path UserSettings(std::string username) const {
    return User(username) / File::Settings();
  }
  path UserLogFiles(std::string username) const {
    return User(username) / Usr::LogFiles();
  }
  path UserSegmentation(std::string username, uint8_t i) const {
    return User(username) / Root::Segmentation(i);
  }
  path UserAnnotations(std::string username, uint8_t i) const {
    return UserSegmentation(username, i) / File::Annotations();
  }
  path UserSegments(std::string username, uint8_t i) const {
    return UserSegmentation(username, i) / Seg::Segments();
  }
  path UserSegmentDataPage(std::string username, uint8_t i,
                           uint8_t page) const {
    return UserSegmentation(username, i) / Seg::SegmentDataPage(page);
  }
  path UserSegmentListTypesPage(std::string username, uint8_t i,
                                uint8_t page) const {
    return UserSegmentation(username, i) / Seg::SegmentListTypesPage(page);
  }
  path UserValidGroupNum(std::string username, uint8_t i) const {
    return UserSegmentation(username, i) / Seg::ValidGroupNum();
  }
  path UserMST(std::string username, uint8_t i) const {
    return UserSegmentation(username, i) / Seg::MST();
  }
  path UserUserEdges(std::string username, uint8_t i) const {
    return UserSegmentation(username, i) / Seg::UserEdges();
  }
  path UserLongRangeConnections(std::string username, uint8_t i) const {
    return UserSegmentation(username, i) / File::LongRangeConnections();
  }

  // Misc

  static path AddOmniExtensionIfNeeded(path fnp) {
    if (extension(fnp) != ".omni") {
      fnp += ".omni";
    }
    return fnp;
  }

  static path TempFileName(const om::utility::UUID& uuid) {
    return tempPath() / ("omni.temp." + uuid.Str());
  }

 private:
  template <typename T> static path toPath(T i) {
    return path(std::to_string(i));
  }

  const path omniFile_;
  path filesFolder_;
};
}
}  // namespace om::file::