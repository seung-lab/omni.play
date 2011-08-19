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
    double threshold_;
    
public:
    userSettings(std::string filename)
        : filename_(filename)
        , defaultThreshold_(0.999)
        , threshold_(defaultThreshold_)
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
        std::cout << "Changing Threshold!\n";
        threshold_ = val;
    }
        
};
    
} // namespace om