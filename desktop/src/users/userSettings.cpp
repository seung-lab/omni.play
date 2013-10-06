#include "yaml-cpp-old/yaml.h"
#include "users/userSettings.h"
#include "datalayer/fs/omFile.hpp"
#include <fstream>

namespace om {

void userSettings::Load() {
  if (om::file::old::exists(filename_)) {
    YAMLold::Node in = YAMLold::LoadFile(filename_);

    threshold_ = in["threshold"].as<double>(defaultThreshold_);
    sizeThreshold_ = in["sizeThreshold"].as<double>(defaultSizeThreshold_);
    showAnnotations_ = in["showAnnotations"].as<bool>(defaultShowAnnotations_);
    alpha_ = in["alpha"].as<double>(0.2);
  }
}

void userSettings::Save() {
  YAMLold::Node n;
  n["threshold"] = threshold_;
  n["sizeThreshold"] = sizeThreshold_;
  n["showAnnotations"] = showAnnotations_;
  n["alpha"] = alpha_;

  std::ofstream out(filename_);
  YAMLold::Emitter e(out);
  e << YAMLold::BeginDoc << n << YAMLold::EndDoc;
}

}  // namespace om
