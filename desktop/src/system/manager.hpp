#pragma once
#include "precomp.h"

#include "common/exception.h"
#include "utility/omLockedPODs.hpp"
#include "utility/yaml/manager.hpp"

namespace om {
namespace system {

template <typename T>
struct ManagedObject {
  om::common::ID ID;
  bool Enabled;
  zi::spinlock Lock;
  T* Object;
};
}
}

namespace YAML {

template <typename T>
struct convert<om::system::ManagedObject<T>> {
  static Node encode(const om::system::ManagedObject<T>& data) {
    Node n;
    n["id"] = data.ID;
    n["enabled"] = data.Enabled;
    n["value"] = *data.Object;
    return n;
  }
  static bool decode(const Node& node, om::system::ManagedObject<T>& data) {
    if (!node.IsMap()) {
      return false;
    }
    try {
      data.ID = node["id"].as<om::common::ID>(0);
      data.Enabled = node["enabled"].as<bool>(true);
      *data.Object = node["value"].as<T>();
    }
    catch (std::exception e) {
      log_errors << "Unable to decode ManagedObject: " << e.what();
      return false;
    }
  }
};
}

namespace om {
namespace system {

template <typename T>
class Manager {
 public:
  typedef ManagedObject<T> obj;
  typedef std::unordered_map<om::common::ID, obj> coll;

 private:
  struct get_value : std::unary_function<typename coll::value_type, obj&> {
    obj& operator()(typename coll::value_type& pair) const {
      return pair.second;
    }
  };

  struct get_enabled {
    bool operator()(obj& o) const { return o.Enabled; }
  };

 public:
  typedef boost::transform_iterator<get_value, typename coll::iterator>
      iterator;
  typedef boost::transform_iterator<get_value, typename coll::const_iterator>
      const_iterator;
  typedef boost::filter_iterator<get_enabled, iterator> enabled_iterator;
  typedef boost::filter_iterator<get_enabled, iterator> const_enabled_iterator;

 private:
  coll objs_;
  size_t next_;
  zi::spinlock lock_;
  LockedBool disposed_;

 public:
  Manager() : objs_(), next_(1) { disposed_.set(false); }

  virtual ~Manager() {
    zi::guard g(lock_);
    if (!disposed_.get()) {
      FOR_EACH(iter, objs_) {
        zi::guard g2(iter->second.Lock);
        delete iter->second.Object;
      }
      disposed_.set(true);
    }
  }

  obj& Add(T* toAdd, size_t id = 0, bool enabled = true) {
    zi::guard g(lock_);
    if (id == 0 || isValid(id)) {
      id = next_;
    }
    auto& obj = objs_[id];
    obj.ID = id;
    obj.Enabled = enabled;
    obj.Object = toAdd;
    findNext();
    return obj;
  }

  bool IsValid(om::common::ID id) {
    zi::guard g(lock_);
    return isValid(id);
  }

  obj& Get(om::common::ID id) {
    zi::guard g(lock_);
    iterator iter = objs_.find(id);
    if (iter != end()) {
      throw om::ArgException("Bad id");
    }
    return iter->second;
  }

  void Remove(om::common::ID id) {
    zi::guard g(lock_);
    if (isValid(id)) {
      zi::guard g2(objs_[id].Lock);
      delete objs_[id].Object;
      objs_.erase(id);
      if (id < next_) {
        next_ = id;
      }
    }
  }

  void Save(YAML::Node& node) const {
    for (auto& iter : objs_) {
      node.push_back(iter.second);
    }
  }

  void Load(const YAML::Node& in) {
    if (in["size"].IsDefined()) {
      auto valid = in["valid set"].as<om::common::SegIDSet>();
      auto enabled = in["enabled set"].as<om::common::SegIDSet>();

      int idx = 0;
      for (auto& i : valid) {
        T* obj = parse(in["values"][idx]);
        Add(obj, i, enabled.count(i) > 0);
        idx++;
      }
    } else {
      for (auto& iter : in) {
        size_t id;
        id = iter["id"].as<om::common::ID>();
        bool enabled;
        enabled = iter["enabled"].as<bool>(true);
        T* obj = parse(iter["value"]);
        Add(obj, id, enabled);
      }
    }
  }

  inline iterator begin() { return iterator(objs_.begin()); }

  inline iterator end() { return iterator(objs_.end()); }

  inline const_iterator begin() const { return const_iterator(objs_.begin()); }

  inline const_iterator end() const { return const_iterator(objs_.end()); }

  struct enabled {
   private:
    Manager<T>* man_;

   public:
    enabled(Manager<T>* man) : man_(man) {}

    inline enabled_iterator begin() {
      return enabled_iterator(man_->begin(), man_->end());
    }

    inline enabled_iterator end() {
      return enabled_iterator(man_->end(), man_->end());
    }

    inline const_enabled_iterator begin() const {
      return const_enabled_iterator(man_->begin(), man_->end());
    }

    inline const_enabled_iterator end() const {
      return const_enabled_iterator(man_->end(), man_->end());
    }
  };

  inline enabled Enabled() { return enabled(this); }

 protected:
  virtual T* parse(const YAML::Node& in) = 0;

  inline bool isValid(om::common::ID id) { return objs_.count(id) > 0; }

  void findNext() {
    for (int i = next_;; ++i) {
      if (!isValid(i)) {
        next_ = i;
        return;
      }
    }
  }
};
}
}  // namespace om::system::
