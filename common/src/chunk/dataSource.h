#pragma once

#include "datalayer/dataSource.hpp"
#include "chunk/dataSources.hpp"
#include "datalayer/filePtrCache.hpp"
#include "datalayer/paths.hpp"

namespace om {
namespace chunk {

class FileDataSource : public ChunkDS {
 public:
  FileDataSource(file::Paths::Vol, const common::DataType&,
                 const coords::VolumeSystem&);

  std::shared_ptr<chunk::ChunkVar> Get(const coords::Chunk&,
                                       bool async = false) const;
  bool Put(const coords::Chunk&, std::shared_ptr<chunk::ChunkVar>,
           bool async = false);

  inline bool is_cache() const { return false; }
  inline bool is_persisted() const { return true; }

 private:
  std::string fileName(int) const;

  const file::Paths::Vol volPaths_;
  const common::DataType dataType_;
  mutable datalayer::VarFilePtrCache files_;
  const coords::VolumeSystem& coordSystem_;
};
}
}  // namespace om::chunk::
