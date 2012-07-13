#pragma once

#include "common/omCommon.h"
#include "utility/yaml/omYaml.hpp"

namespace om {
    

// TODO: Locking?
class userSettings
{
private:   
    const std::string filename_;
    const double defaultThreshold_;
    const double defaultSizeThreshold_;
    const double defaultASThreshold_;
    double threshold_;
    double sizeThreshold_;
    double automaticSpreadingThreshold_;
    
public:
    userSettings(std::string filename)
        : filename_(filename)
        , defaultThreshold_(0.999)
        , defaultSizeThreshold_(0)
        , defaultASThreshold_(0.999)
        , threshold_(defaultThreshold_)
        , sizeThreshold_(defaultSizeThreshold_)
        , automaticSpreadingThreshold_(defaultASThreshold_)
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
    
    inline float getASThreshold()
    {
        return automaticSpreadingThreshold_;
    }

    inline void setASThreshold(float val)
    {
        automaticSpreadingThreshold_ = val;
    }
};
    
} // namespace om


/// Working; But why resetting all three thresholds every time something is changes?