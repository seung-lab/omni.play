#pragma once

namespace om {
namespace tiles {

template <typename T>
class filters {
private:
    const int chunkDim_;
    const int elementsPerTile_;

public:
    filters(const int chunkDim)
        : chunkDim_(chunkDim)
        , elementsPerTile_(chunkDim*chunkDim)
    {}

    void Brightness(boost::shared_ptr<T> tile, const T absMax, const int32_t shift)
    {
        zi::transform(tile->GetData(),
                      tile->GetData() + elementsPerTile_,
                      tile->GetData(),
                      ChangeBrightness<T>(absMax, shift));
    }

    void Contrast(boost::shared_ptr<T> tile, const T absMax, const double contrast)
    {
        zi::transform(tile->GetData(),
                      tile->GetData() + elementsPerTile_,
                      tile->GetData(),
                      ChangeContrast<T>(absMax, contrast));
    }

    void Gamma(boost::shared_ptr<T> tile, const double gamma)
    {
        zi::transform(tile->GetData(),
                      tile->GetData() + elementsPerTile_,
                      tile->GetData(),
                      ChangeGamma<T>(gamma));
    }

    template <typename C>
    boost::shared_ptr<C> recast(boost::shared_ptr<T> oldTile) const
    {
        boost::shared_ptr<C> ret = new boost::shared_ptr<C>();

        std::copy(oldTile->GetData(),
                  oldTile->GetData() + elementsPerTile_,
                  ret->GetData());

        return ret;
    }

    inline boost::shared_ptr<uint8_t> recastToUint8(boost::shared_ptr<T> oldTile) const {
        return recast<uint8_t>(oldTile);
    }

    inline boost::shared_ptr<uint32_t> recastToUint32(boost::shared_ptr<T> oldTile) const {
        return recast<uint32_t>(oldTile);
    }

    template <typename C>
    boost::shared_ptr<C> rescaleAndCast(boost::shared_ptr<T> oldTile,
                                        const T rangeMin, const T rangeMax,
                                        const T absMax) const
    {
        boost::shared_ptr<C> ret = new boost::shared_ptr<C>();

        zi::transform(oldTile->GetData(),
                      oldTile->GetData() + elementsPerTile_,
                      ret->GetData(),
                      Rescale<T,C>(rangeMin, rangeMax, absMax));

        return ret;
    }

private:
    template <typename U, typename C>
    class Rescale {
    private:
        const U rangeMin_, rangeMax_, absMax_;

    public:
        Rescale(const U rangeMin, const U rangeMax, const U absMax)
            : rangeMin_(rangeMin)
            , rangeMax_(rangeMax)
            , absMax_(absMax)
        {}

        C operator()(U val) const {
            return (val-rangeMin_) * ((double)absMax_/(rangeMax_-rangeMin_));
        }
    };

    // brightness, contrast, and gamma based on
    // http://pippin.gimp.org/image_processing/chap_point.html#id2556807

    // TODO: clamp should not be needed

    template <typename U>
    inline static T clamp(T absMax, U val)
    {
        if(val > absMax) {
            return absMax;
        }

        if(val < 0) {
            return 0;
        }

        return val;
    }

    template <typename U>
    class ChangeBrightness {
    public:
        ChangeBrightness(const U absMax, const int32_t shift)
            : absMax_(absMax)
            , shift_(shift)
        {}

        U operator()(U val) const
        {
            if(!val){
                return 0;
            }

            return clamp(absMax_, val + shift_);
        }
    private:
        const U absMax_;
        int32_t shift_;
    };

    template <typename U>
    class ChangeContrast {
    public:
        ChangeContrast(const double absMax, const double contrast)
            : absMax_(absMax)
            , halfAbsMax_(absMax_/2.0)
            , contrast_(contrast)
        {}

        U operator()(U val) const {
            return clamp(255, (static_cast<double>(val) - halfAbsMax_) * contrast_ + halfAbsMax_);
        }

    private:
        const double absMax_;
        const double halfAbsMax_;
        const double contrast_;
    };

    template <typename U>
    class ChangeGamma {
    public:
        ChangeGamma(const double gamma)
            : gamma_(gamma)
        {}

        U operator()(U val) const {
            return clamp(255, 255.0*std::pow(static_cast<double>(val)/255.0, gamma_));
        }
    private:
        const double gamma_;
    };
};

}
}
