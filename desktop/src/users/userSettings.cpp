#include "users/userSettings.h"

#include "datalayer/file.h"

namespace om {

void userSettings::Load() {
  using namespace YAML;

  if (om::file::exists(filename_)) {
    Node in = YAML::LoadFile(filename_);

    threshold_ = in["threshold"].as<double>(defaultThreshold_);
    sizeThreshold_ = in["sizeThreshold"].as<double>(defaultSizeThreshold_);
    showAnnotations_ = in["showAnnotations"].as<bool>(defaultShowAnnotations_);
  }
}

void userSettings::Save() {
  using namespace YAML;

  try {
    std::ofstream fout(filename_);
    Node n;
    n["threshold"] = threshold_;
    n["sizeThreshold"] = sizeThreshold_;
    n["showAnnotations"] = showAnnotations_;
    fout << n;
  }
  catch (std::exception e) {
    log_errors << "Error writing userSettings: " << e.what();
  }
}

}  // namespace om
