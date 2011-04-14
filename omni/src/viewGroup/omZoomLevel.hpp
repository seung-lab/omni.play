#ifndef OM_ZOOM_LEVEL_HPP
#define OM_ZOOM_LEVEL_HPP

class OmZoomLevel {
private:
    bool valid;
    int mipLevel_;
    float zoomFactor_;

public:
    OmZoomLevel()
        : valid(false)
        , mipLevel_(0)
        , zoomFactor_(0.6)
    {}

    void Update(const int defaultMipLevel)
    {
        if(!valid){
            Reset(defaultMipLevel);
        }
    }

    void Reset(const int defaultMipLevel)
    {
        mipLevel_ = defaultMipLevel;
        zoomFactor_ = 1;
        valid = true;
    }

    float GetZoomScale() const {
        return zoomFactor_;
    }

    int GetMipLevel() const {
        return mipLevel_;
    }

    void MouseWheelZoom(const int numSteps, const bool isLevelLocked,
                        const int maxMipLevel)
    {
        if (numSteps >= 0){
            mouseWheelZoomIn(numSteps, isLevelLocked);
        } else{
            mouseWheelZoomOut(numSteps, isLevelLocked, maxMipLevel);
        }
    }

private:
    void mouseWheelZoomOut(const int numSteps, const bool isLevelLocked,
                           const int maxMipLevel)
    {
        zoomFactor_ /= std::pow(1.125, -numSteps); // numSteps is negative!

        if( zoomFactor_ < 0.1 ){
            zoomFactor_ = 0.1;
        }

        if(isLevelLocked){
            return;
        }

        if(zoomFactor_ <= 0.6 && mipLevel_ < maxMipLevel)
        {
            mipLevel_ += 1; // move to next mip level
            zoomFactor_ = 1.0;
        }
    }

    void mouseWheelZoomIn(const int numSteps, const bool isLevelLocked)
    {
        // Blanchette/Summerfield second edition page 482
        zoomFactor_ *= std::pow(1.125, numSteps);

        if(isLevelLocked){
            return;
        }

        if(zoomFactor_ >= 1 && mipLevel_ > 0)
        {
            mipLevel_ -= 1; //move to previous mip level
            zoomFactor_ = 0.6;
        }
    }
};

#endif
