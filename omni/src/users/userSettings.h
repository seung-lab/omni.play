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
    double threshold_;
    double sizeThreshold_;
    
public:
    userSettings(std::string filename)
        : filename_(filename)
        , defaultThreshold_(0.999)
        , defaultSizeThreshold_(0)
        , threshold_(defaultThreshold_)
        , sizeThreshold_(defaultSizeThreshold_)
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
        
};
    
} // namespace om