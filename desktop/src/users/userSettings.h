#pragma once
#include "precomp.h"

#include "common/common.h"
#include "utility/yaml/omYaml.hpp"

namespace om {

// TODO: Locking?
class userSettings {
 private:
  const std::string filename_;
  const double defaultThreshold_;
  const double defaultSizeThreshold_;
  const bool defaultShowAnnotations_;
  const double defaultGrowThreshold_;
  double threshold_;
  double sizeThreshold_;
  double growThreshold_;
  int sizeThreshold_direction_;
  bool showAnnotations_;

 public:
  userSettings(std::string filename)
      : filename_(filename),
        defaultThreshold_(0.999),
        defaultSizeThreshold_(250),
        defaultShowAnnotations_(false),
        defaultGrowThreshold_(0.999),
        threshold_(defaultThreshold_),
        sizeThreshold_(defaultSizeThreshold_),
        growThreshold_(defaultGrowThreshold_),
        sizeThreshold_direction_(0),
        showAnnotations_(defaultShowAnnotations_) {
    log_infos << "New User Settings";
  }

  void Load();

  void Save();

  inline std::string getFilename() { return filename_; }

  inline double getThreshold() { return threshold_; }

  inline void setThreshold(double val) { threshold_ = val; }

  inline double getSizeThreshold() { return sizeThreshold_; }

  inline double setSizeThreshold(double val) { sizeThreshold_ = val; } 

  inline bool getAnnotationVisible() { return showAnnotations_; }

  inline void setAnnotationVisible(bool val) { showAnnotations_ = val; }

  inline double getGrowThreshold() { return growThreshold_; }

  inline void setGrowThreshold(double val) { growThreshold_ = val; }
};

}  // namespace om


/// Working; But why resetting all three thresholds every time something is changes?
