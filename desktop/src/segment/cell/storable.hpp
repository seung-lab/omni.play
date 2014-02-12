#pragma once

#include <vector>
#include <map>

namespace om {

// data that can be easily stored on disk, in cloud, etc., somehow...
class Storable {
 private:
  typedef std::string key_t;
  std::map<key_t, std::string> strings;
  std::map<key_t, bool> bools;
  std::map<key_t, int8_t> int8s;
  std::map<key_t, uint8_t> uint8s;
  std::map<key_t, int32_t> int32s;
  std::map<key_t, uint32_t> uint32s;
  std::map<key_t, int64_t> int64s;
  std::map<key_t, uint64_t> uint64s;
  std::map<key_t, std::vector<std::string>> vec_strings;
  std::map<key_t, std::vector<int8_t>> vec_int8s;
  std::map<key_t, std::vector<uint8_t>> vec_uint8s;
  std::map<key_t, std::vector<int32_t>> vec_int32s;
  std::map<key_t, std::vector<uint32_t>> vec_uint32s;
  std::map<key_t, std::vector<int64_t>> vec_int64s;
  std::map<key_t, std::vector<uint64_t>> vec_uint64s;
  std::map<key_t, Storable> storables;

 public:
  // TODO: not sure of cleanest way yet to add data...
};

}  // om::
