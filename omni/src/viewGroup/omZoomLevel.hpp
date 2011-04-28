#ifndef OM_ZOOM_LEVEL_HPP
#define OM_ZOOM_LEVEL_HPP

class OmZoomLevel {
private:
    /**
     * for n = mipLevel:
     *
     *  for zoom out:  imageSize                  imageSize
     *                ---------- * zoomFactor == ----------- * newZoomFactor
     *                   2^n                       2^(n+1)
     *
     *   ==> newZoomFactor = 2*zoomFactor
     *
     *
     *  for zoom in:  imageSize                   imageSize
     *               ----------- * zoomFactor == ----------- * newZoomFactor
     *                   2^n                       2^(n-1)
     *
     *   ==> newZoomFactor = zoomFactor/2
     *
     **/

    /**
     * pivot point that determines when mip level changes during zoom in/out
     *
     *   --decrease to use lower (less downsampled) mip levels at a given viewing distance
     *       (and increase number of tiles needed to display slice)
     *
     *   --increase to use higher (more downsampled) mip levels at a given viewing distance
     *       (and decrease the number of tiles needed to display slice)
     **/
    // static const double zoomPivotPoint_ = 2.5;
    static const double zoomPivotPoint_ = 1.0;

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

        if( zoomFactor_ < 0.1 ){  // mipLevel == 0
            zoomFactor_ = 0.1;
        }

        if(isLevelLocked){
            return;
        }

        if(zoomFactor_ <= (zoomPivotPoint_ / 2.0) && mipLevel_ < maxMipLevel)
        {
            // move to next mip level
            mipLevel_ += 1;
            zoomFactor_ = zoomPivotPoint_;
        }
    }

    void mouseWheelZoomIn(const int numSteps, const bool isLevelLocked)
    {
        // Blanchette/Summerfield second edition page 482
        zoomFactor_ *= std::pow(1.125, numSteps);

        if(isLevelLocked){
            return;
        }

        if(zoomFactor_ >= zoomPivotPoint_ && mipLevel_ > 0)
        {
            // move to previous mip level
            mipLevel_ -= 1;
            zoomFactor_ = zoomPivotPoint_ / 2.0;
        }
    }
};

#endif
