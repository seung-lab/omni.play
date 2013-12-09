#pragma once

#include <boost/shared_ptr.hpp>

namespace om {
namespace datalayer {

/**
 * IDataSource models a map as a source of data organized by key:value.
 */
template <typename TKey, typename TValue> class IDataSource {
 public:
  virtual ~IDataSource() {}
  virtual std::shared_ptr<TValue> Get(const TKey&, bool async = false) = 0;
  virtual bool Put(const TKey&, std::shared_ptr<TValue>,
                   bool asnyc = false) = 0;
  virtual void Flush() {}

  virtual bool is_cache() const = 0;
  virtual bool is_persisted() const = 0;
};
}
}  // namespace om::datalayer::
