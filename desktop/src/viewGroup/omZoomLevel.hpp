#pragma once
#include <events/omEvents.h>
#include <cmath>

class OmZoomLevel {
private:
    bool valid;
    int mipLevel_;
    int maxMipLevel_;
    double zoomFactor_;

public:
    OmZoomLevel()
        : valid(false)
        , mipLevel_(0)
        , maxMipLevel_(0)
        , zoomFactor_(0.6)
    {}

    void Update(const int maxMipLevel)
    {
        if(!valid){
            Reset(maxMipLevel);
        }

        if(maxMipLevel > maxMipLevel_){
            maxMipLevel_ = maxMipLevel;
        }
    }

    void Reset(const int defaultMipLevel)
    {
        mipLevel_ = defaultMipLevel;
        zoomFactor_ = 1;
        setMipLevel();
        valid = true;
    }

    double GetZoomScale() const {
        return zoomFactor_;
    }

    int GetMipLevel() const {
        return mipLevel_;
    }

    void MouseWheelZoom(const int numSteps, const bool isLevelLocked)
    {
        if (numSteps >= 0){
            mouseWheelZoomIn(numSteps, isLevelLocked);

        } else{
            mouseWheelZoomOut(numSteps, isLevelLocked);
        }
    }

private:
    void mouseWheelZoomOut(const int numSteps, const bool isLevelLocked)
    {
        zoomFactor_ /= std::pow(1.125, -numSteps); // numSteps is negative!

        if( zoomFactor_ < 0.1 ){  // mipLevel == 0
            zoomFactor_ = 0.1;
        }

        if(isLevelLocked){
            return;
        }

        setMipLevel();
    }

    void mouseWheelZoomIn(const int numSteps, const bool isLevelLocked)
    {
        // Blanchette/Summerfield second edition page 482
        zoomFactor_ *= std::pow(1.125, numSteps);

        if(isLevelLocked){
            return;
        }

        setMipLevel();
    }

    void setMipLevel()
    {
        mipLevel_ = log2(3 / zoomFactor_);
        if(mipLevel_ < 0) {
            mipLevel_ = 0;
        } else if(mipLevel_ > maxMipLevel_) {
            mipLevel_ = maxMipLevel_;
        }
    }
};

