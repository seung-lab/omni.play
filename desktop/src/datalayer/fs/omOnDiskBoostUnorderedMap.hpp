#pragma once
#include "precomp.h"

// based on
// http://www.boost.org/doc/libs/1_38_0/doc/html/interprocess/allocators_containers.html#interprocess.allocators_containers.additional_containers.unordered

#include "utility/omUUID.hpp"
#include "datalayer/fs/omOnDiskBoostUnorderedMap.hpp"

namespace bi = boost::interprocess;

static const uint64_t InitialNumMB = 10;
static const uint64_t InitialNumBytes = om::math::bytesPerMB * InitialNumMB;
static const uint64_t MinNumBytesBeforeGrowing = 20000;

template <class KEY, class VAL>
class OmOnDiskBoostUnorderedMap {
 private:
  const OmUUID uuid_;
  const std::string fnp_;

  std::string makeFileName() const { return OmFileNames::TempFileName(uuid_); }

 public:
  OmOnDiskBoostUnorderedMap() : uuid_(OmUUID()), fnp_(makeFileName()) {
    create();
  }

  ~OmOnDiskBoostUnorderedMap() { om::file::rmFile(fnp_); }

  void insert(const KEY& key, const VAL& val) {
    if (file_->get_free_memory() < MinNumBytesBeforeGrowing) {
      grow();
    }

    hash_->insert(ValueType(key, val));
  }

  VAL& at(const KEY& key) { return hash_->at(key); }

 private:
  typedef std::pair<const KEY, VAL> ValueType;

  typedef bi::allocator<ValueType, bi::managed_mapped_file::segment_manager>
      allocator_t;

  typedef std::unordered_map<KEY, VAL, boost::hash<KEY>, std::equal_to<KEY>,
                             allocator_t> onDiskHash_t;

  typedef typename onDiskHash_t::iterator iterator_t;

  onDiskHash_t* hash_;

  std::shared_ptr<bi::managed_mapped_file> file_;

  void create() {
    file_ = std::make_shared<bi::managed_mapped_file>(
        bi::create_only, fnp_.c_str(), InitialNumBytes);

    hash_ = file_->construct<onDiskHash_t>("MyHashMap")(
        100000, boost::hash<int>(), std::equal_to<int>(),
        file_->get_allocator<VAL>());
  }

  void open() {
    file_ =
        std::make_shared<bi::managed_mapped_file>(bi::open_only, fnp_.c_str());

    hash_ = file_->find_or_construct<onDiskHash_t>("MyHashMap")(
        100000, boost::hash<int>(), std::equal_to<int>(),
        file_->get_allocator<VAL>());
  }

  void close() {
    hash_ = nullptr;
    file_.reset();
  }

  void grow() {
    close();

    const uint64_t oldSize = om::file::numBytes(fnp_);
    const uint64_t newSize = 5 * oldSize;

    log_infos << "\tgrowing file from " << om::string::bytesToMB(oldSize)
              << " to " << om::string::bytesToMB(newSize) << "..."
              << std::flush;

    if (!bi::managed_mapped_file::grow(fnp_.c_str(), newSize)) {
      throw om::IoException("could not resize file");
    }

    log_infos << "done";

    open();
  }
};
