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

    bool valid;
    int mipLevel_;
    int maxMipLevel_;
    double zoomFactor_;

    /**
     * pivot point that determines when mip level changes during zoom in/out
     *
     *   --decrease to use lower (less downsampled) mip levels at a given viewing distance
     *       (and increase number of tiles needed to display slice)
     *
     *   --increase to use higher (more downsampled) mip levels at a given viewing distance
     *       (and decrease the number of tiles needed to display slice)
     **/
    double zoomPivotPoint_;

public:
    OmZoomLevel()
        : valid(false)
        , mipLevel_(0)
        , maxMipLevel_(0)
        , zoomFactor_(0.6)
        , zoomPivotPoint_(1.0)
    {}

    void Update(const int maxMipLevel)
    {
        if(!valid){
            Reset(maxMipLevel);
        }

        if(maxMipLevel > maxMipLevel_){
            maxMipLevel_ = maxMipLevel;
            resetZoomPivotPoint();
        }
    }

    void Reset(const int defaultMipLevel)
    {
        mipLevel_ = defaultMipLevel;
        zoomFactor_ = 1;
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

    void resetZoomPivotPoint()
    {
        if(maxMipLevel_ > 1){
            zoomPivotPoint_ = 2.55;
        } else {
            zoomPivotPoint_ = 1.0;
        }
    }

    void mouseWheelZoomOut(const int numSteps, const bool isLevelLocked)
    {
        zoomFactor_ /= std::pow(1.125, -numSteps); // numSteps is negative!

        if( zoomFactor_ < 0.1 ){  // mipLevel == 0
            zoomFactor_ = 0.1;
        }

        if(isLevelLocked){
            return;
        }

        if(zoomFactor_ <= (zoomPivotPoint_ / 2.0) && mipLevel_ < maxMipLevel_)
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
