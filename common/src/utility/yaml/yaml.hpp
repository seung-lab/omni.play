#pragma once

#include "precomp.h"
#include "common/common.h"
#include "yaml-cpp/yaml.h"
#include "boost/filesystem.hpp"
#include "boost/optional.hpp"

namespace om {
namespace yaml {

class Util {
 public:
  // based on
  // http://code.google.com/p/yaml-cpp/wiki/HowToParseADocument#A_Complete_Example
  template <typename T>
  static std::vector<T> Parse(const std::string& fnp) {
    YAML::Node doc;
    Read(fnp, doc);

    std::vector<T> ret;

    for (auto& n : doc) {
      ret.push_back(n.as<T>());
    }

    return ret;
  }

  // based on
  // http://code.google.com/p/yaml-cpp/wiki/HowToParseADocument#A_Complete_Example
  static void Read(const std::string& fnp, YAML::Node& node) {
    if (!boost::filesystem::exists(fnp)) {
      throw IoException("could not find file", fnp);
    }

    node = YAML::LoadFile(fnp);
  }

  // based on
  // http://code.google.com/p/yaml-cpp/wiki/HowToParseADocument#A_Complete_Example
  static void Write(const std::string& fnp, YAML::Emitter& emitter) {
    std::ofstream fout(fnp.c_str());

    fout << emitter.c_str();

    fout.close();
  }
};

}  // namespace yaml
}  // namespace om
