#pragma once

#include "common/common.h"
#include "utility/yaml/omYaml.hpp"

namespace om {


// TODO: Locking?
class userSettings
{
private:
    const std::string filename_;
    const double defaultThreshold_;
    const double defaultSizeThreshold_;
    const bool defaultShowAnnotations_;
    double threshold_;
    double sizeThreshold_;
    bool showAnnotations_;

public:
    userSettings(std::string filename)
        : filename_(filename)
        , defaultThreshold_(0.999)
        , defaultSizeThreshold_(250)
        , defaultShowAnnotations_(false)
        , threshold_(defaultThreshold_)
        , sizeThreshold_(defaultSizeThreshold_)
        , showAnnotations_(defaultShowAnnotations_)
    {
        std::cout << "New User Settings\n";
    }

    void Load();

    void Save();

    inline std::string getFilename() {
        return filename_;
    }

    inline float getThreshold() {
        return threshold_;
    }

    inline void setThreshold(float val) {
        threshold_ = val;
    }

    inline float getSizeThreshold() {
        return sizeThreshold_;
    }

    inline void setSizeThreshold(float val) {
        sizeThreshold_ = val;
    }

    inline float getAnnotationVisible() {
        return showAnnotations_;
    }

    inline void setAnnotationVisible(float val) {
        showAnnotations_ = val;
    }

};

} // namespace om
