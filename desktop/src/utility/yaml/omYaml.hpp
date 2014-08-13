#pragma once
#include "precomp.h"

#include "datalayer/file.h"

namespace om {
namespace yaml {

class util {
 public:
  // based on
  // http://code.google.com/p/yaml-cpp/wiki/HowToParseADocument#A_Complete_Example
  template <typename T>
  static std::vector<T> Parse(const std::string& fnp) {
    if (!om::file::exists(fnp)) {
      throw om::IoException("could not find file");
    }

    YAML::Node doc = YAML::LoadFile(fnp);

    std::vector<T> ret;

    for (uint32_t i = 0; i < doc.size(); ++i) {
      T newDoc;
      doc[i] >> newDoc;
      ret.push_back(newDoc);
    }

    return ret;
  }
};

}  // namespace yaml
}  // namespace om
