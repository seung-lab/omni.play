
#include "yaml-cpp/yaml.h"
#include "users/userSettings.h"
#include "datalayer/file.h"
#include <fstream>

namespace om {

void userSettings::Load() {
  if (om::file::exists(filename_)) {
    YAML::Node in = YAML::LoadFile(filename_.string());

    threshold_ = in["threshold"].as<double>(defaultThreshold_);
    sizeThreshold_ = in["sizeThreshold"].as<double>(defaultSizeThreshold_);
    showAnnotations_ = in["showAnnotations"].as<bool>(defaultShowAnnotations_);
    alpha_ = in["alpha"].as<double>(defaultAlpha_);
  }
}

void userSettings::Save() {
  YAML::Node n;
  n["threshold"] = threshold_;
  n["sizeThreshold"] = sizeThreshold_;
  n["showAnnotations"] = showAnnotations_;
  n["alpha"] = alpha_;

  std::ofstream out(filename_.string());
  YAML::Emitter e(out);
  e << YAML::BeginDoc << n << YAML::EndDoc;
}

}  // namespace om
