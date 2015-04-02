#pragma once

#include "datalayer/memMappedFile.hpp"
#include "datalayer/dataSource.hpp"
#include "datalayer/vector.hpp"
#include "datalayer/compressedFile.h"
#include "datalayer/file.h"

namespace om {
namespace datalayer {

template <typename TKey, typename TValue>
class VectorFileDataSource
    : public virtual IDataSource<TKey, Vector<TKey, TValue>> {
 public:
  VectorFileDataSource(file::path root, size_t size = 0)
      : root_(root), size_(size) {}

  virtual std::shared_ptr<Vector<TKey, TValue>> Get(const TKey& key, bool async = false) {
    return doGet(key, async);
  }

  virtual std::shared_ptr<Vector<TKey, TValue>> Get(const TKey& key, bool async = false) const {
    return doGet(key, async);
  }

  virtual bool Put(const TKey& key, std::shared_ptr<Vector<TKey, TValue>> data,
                   bool async = false) {
    resize(data->Values);
    try {
      doPut(key, data);
    }
    catch (Exception& e) {

      log_errors << "Unable to Write FileDataSource: " << e.what();
      return false;
    }
    return true;
  }

  virtual bool is_cache() const { return false; }
  virtual bool is_persisted() const { return true; }

 protected:
  virtual std::shared_ptr<Vector<TKey, TValue>> doGet(
      const TKey& key, bool async = false) const {
    try {
      auto ret = std::make_shared<Vector<TKey, TValue>>(key);
      file::readAll(fnp(key), ret->Values);
      resize(ret->Values);

      return ret;
    }
    catch (Exception& e) {
      log_errors << "Unable to Load FileDataSource: " << e.what();
      std::shared_ptr<Vector<TKey, TValue> > new_vector;
      new_vector.reset(new Vector<TKey, TValue>(key));
      return new_vector;
    }
  }

  virtual void doPut(const TKey& key,
                     std::shared_ptr<Vector<TKey, TValue>> data) {
    file::writeAll(fnp(key), data->Values);
  }

  void resize(std::vector<TValue>& v) const {
    if (size_ > 0 && v.size() != size_) {
      v.resize(size_);
    }
  }

  virtual file::path relativePath(const TKey& key) const = 0;

  virtual file::path fnp(const TKey& key) const {
    return root_ / relativePath(key);
  }

  file::path root_;
  size_t size_;
};

template <typename TKey, typename TValue>
class CompressedVectorFileDataSource
    : public VectorFileDataSource<TKey, TValue> {
 private:
  typedef VectorFileDataSource<TKey, TValue> base_t;

 public:
  CompressedVectorFileDataSource(file::path root, size_t size = 0)
      : base_t(root, size), tempDir_(file::tempFile()) {
    file::MkDir(tempDir_);
  }

  ~CompressedVectorFileDataSource() { file::RemoveDir(tempDir_); }

 protected:
  virtual std::shared_ptr<Vector<TKey, TValue>> doGet(const TKey& key,
                                                      bool async = false) const
      override {
    try {
      file::uncompressFileToFile(base_t::fnp(key).string(), fnp(key).string());
      return base_t::doGet(key, async);
    }
    catch (Exception e) {
      file::CopyFile(base_t::fnp(key), fnp(key), true);
      return base_t::doGet(key, async);
    }
  }

  virtual void doPut(const TKey& key,
                     std::shared_ptr<Vector<TKey, TValue>> data) override {
    file::compressToFileVector(data->Values, fnp(key).string());
    file::CopyFile(fnp(key).string(), base_t::fnp(key).string(), true);
  }

  virtual file::path fnp(const TKey& key) const {
    return tempDir_ / relativePath(key);
  }

  virtual file::path relativePath(const TKey& key) const = 0;

  file::path tempDir_;
};
}
}  // namespace om::datalayer::
