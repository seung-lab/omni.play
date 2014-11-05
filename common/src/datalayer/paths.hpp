#pragma once

#include "precomp.h"
#include "datalayer/file.h"
#include "common/enums.hpp"
#include "coordinates/chunk.h"
#include "utility/UUID.hpp"

namespace om {
namespace file {

class Paths {
 public:
  Paths(const path& file, bool newFile=false);

  static bool IsValid(const path& file);
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
    static path MeshMetaData() { return "meshMetadata.ver1"; }
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
    static path AbsCoord() { return "abs_coord.ver1"; }
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
    static path DataRel(uint8_t mipLevel, const common::DataType& type) {
      return Mip(mipLevel) / File::Volume(type);
    }

    static path AbsCoordRel(uint8_t mipLevel) {
      return Mip(mipLevel) / File::AbsCoord();
    }

    path Data(uint8_t mipLevel, const common::DataType& type) const {
      return root_ / DataRel(mipLevel, type);
    }

    path AbsCoord(uint8_t mipLevel) const {
      return root_ / AbsCoordRel(mipLevel);
    }

    Vol() {}
    Vol(om::file::path root) : root_(root) {}

    operator path() const { return root_; }

   protected:
    om::file::path root_;
  };

  // Relative to Segmentation
  struct Seg : public Vol {
    static path ChunksRel() { return "chunks"; }
    static path ChunkRel(const coords::Chunk& c) {
      return ChunksRel() / Paths::Chunk(c);
    }
    static path ChunkUniqueValuesRel(const coords::Chunk& c) {
      return ChunkRel(c) / File::UniqueValues();
    }
    static path MeshesRel() { return "meshes/1.0000"; }
    static path MeshMetaDataRel() { return MeshesRel() / File::MeshMetaData(); }
    static path MeshAllocTableRel(const coords::Chunk& c) {
      return MeshesRel() / Paths::Chunk(c) / File::MeshAllocTable();
    }
    static path MeshDataRel(const coords::Chunk& c) {
      return MeshesRel() / Paths::Chunk(c) / File::MeshData();
    }
    static path SegmentsRel() { return "segments"; }
    static path SegmentDataPageRel(uint8_t page) {
      return SegmentsRel() / File::SegmentDataPage(page);
    }
    static path SegmentListTypesPageRel(uint8_t page) {
      return SegmentsRel() / File::SegmentListTypesPage(page);
    }
    static path ValidGroupNumRel() {
      return SegmentsRel() / File::ValidGroupNum();
    }
    static path MSTRel() { return SegmentsRel() / File::MST(); }
    static path UserEdgesRel() { return SegmentsRel() / File::UserEdges(); }

    path Chunks() const { return root_ / ChunksRel(); }
    path Chunk(const coords::Chunk& c) const { return root_ / ChunkRel(c); }
    path ChunkUniqueValues(const coords::Chunk& c) const {
      return root_ / ChunkUniqueValuesRel(c);
    }

    path Meshes() const { return root_ / MeshesRel(); }
    path MeshMetaData() const { return root_ / MeshMetaDataRel(); }
    path MeshAllocTable(const coords::Chunk& c) const {
      return root_ / MeshAllocTableRel(c);
    }
    path MeshData(const coords::Chunk& c) const {
      return root_ / MeshDataRel(c);
    }

    path Segments() const { return root_ / SegmentsRel(); }
    path SegmentDataPage(uint8_t page) const {
      return root_ / SegmentDataPageRel(page);
    }
    path SegmentListTypesPage(uint8_t page) const {
      return root_ / SegmentListTypesPageRel(page);
    }
    path ValidGroupNum() const { return root_ / ValidGroupNumRel(); }
    path MST() const { return root_ / MSTRel(); }
    path UserEdges() const { return root_ / UserEdgesRel(); }

    Seg() : Vol() {}
    Seg(path root) : Vol(root) {}
    operator path() const { return root_; }
  };

  struct Usr {
    static path LogFilesRel() { return "logFiles"; }

    path Settings() const { return root_ / File::Settings(); }
    path LogFiles() const { return root_ / LogFilesRel(); }
    path Segmentation(uint8_t i) const { return root_ / Root::Segmentation(i); }
    path Annotations(uint8_t i) const {
      return Segmentation(i) / File::Annotations();
    }
    path Segments(uint8_t i) const {
      return Segmentation(i) / Seg::SegmentsRel();
    }
    path SegmentDataPage(uint8_t i, uint8_t page) const {
      return Segmentation(i) / Seg::SegmentDataPageRel(page);
    }
    path SegmentListTypesPage(uint8_t i, uint8_t page) const {
      return Segmentation(i) / Seg::SegmentListTypesPageRel(page);
    }
    path ValidGroupNum(uint8_t i) const {
      return Segmentation(i) / Seg::ValidGroupNumRel();
    }
    path MST(uint8_t i) const { return Segmentation(i) / Seg::MSTRel(); }
    path UserEdges(uint8_t i) const {
      return Segmentation(i) / Seg::UserEdgesRel();
    }
    path LongRangeConnections(uint8_t i) const {
      return Segmentation(i) / File::LongRangeConnections();
    }

    Usr(path root) : root_(root) {}
    operator path() { return root_; }

   private:
    om::file::path root_;
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
  Vol ChannelPaths(uint8_t i) const { return Vol(Channel(i)); }

  path Segmentations() const { return FilesFolder() / Root::Segmentations(); }
  path Segmentation(uint8_t i) const {
    return FilesFolder() / Root::Segmentation(i);
  }
  Seg SegmentationPaths(uint8_t i) const { return Seg(Segmentation(i)); }

  // Users
  path Users() const { return FilesFolder() / Root::Users(); }
  path User(std::string username) const {
    return FilesFolder() / Root::User(username);
  }
  Usr UserPaths(std::string userName) const { return Usr(User(userName)); }

  // Misc
  static path CookieFile() { return "~/.omni/cookies"; }

  static path AddOmniExtensionIfNeeded(path fnp) {
    if (fnp.extension() != ".omni") {
      fnp += ".omni";
    }
    return fnp;
  }

  static path TempFileName(const om::utility::UUID& uuid) {
    return tempPath() / ("omni.temp." + uuid.Str());
  }

 private:
  template <typename T>
  static path toPath(T i) {
    return path(std::to_string(i));
  }

  path omniFile_;
  path filesFolder_;
};
}
}  // namespace om::file::
