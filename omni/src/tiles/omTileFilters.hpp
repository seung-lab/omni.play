#ifndef OM_TILE_FILTERS_HPP
#define OM_TILE_FILTERS_HPP

#include "tiles/omPooledTile.hpp"

template <typename T>
class OmTileFilters {
private:
    const int chunkDim_;
    const int elementsPerTile_;

public:
    OmTileFilters(const int chunkDim)
        : chunkDim_(chunkDim)
        , elementsPerTile_(chunkDim*chunkDim)
    {}

    void Brightness(OmPooledTile<T>* tile, const T absMax, const int32_t shift)
    {
        zi::transform(tile->GetData(),
                      tile->GetData() + elementsPerTile_,
                      tile->GetData(),
                      ChangeBrightness<T>(absMax, shift));
    }

    void Contrast(OmPooledTile<T>* tile, const T absMax, const double contrast)
    {
        zi::transform(tile->GetData(),
                      tile->GetData() + elementsPerTile_,
                      tile->GetData(),
                      ChangeContrast<T>(absMax, contrast));
    }

    void Gamma(OmPooledTile<T>* tile, const double gamma)
    {
        zi::transform(tile->GetData(),
                      tile->GetData() + elementsPerTile_,
                      tile->GetData(),
                      ChangeGamma<T>(gamma));
    }

    template <typename C>
    OmPooledTile<C>* recast(OmPooledTile<T>* oldTile) const
    {
        OmPooledTile<C>* ret = new OmPooledTile<C>();

        std::copy(oldTile->GetData(),
                  oldTile->GetData() + elementsPerTile_,
                  ret->GetData());

        return ret;
    }

    inline OmPooledTile<uint8_t>* recastToUint8(OmPooledTile<T>* oldTile) const {
        return recast<uint8_t>(oldTile);
    }

    inline OmPooledTile<uint32_t>* recastToUint32(OmPooledTile<T>* oldTile) const {
        return recast<uint32_t>(oldTile);
    }

    template <typename C>
    OmPooledTile<C>* rescaleAndCast(OmPooledTile<T>* oldTile,
                                    const T rangeMin, const T rangeMax,
                                    const T absMax) const
    {
        OmPooledTile<C>* ret = new OmPooledTile<C>();

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

    template<typename U>
    static T clamp(T absMax, U val) {
        if(val > absMax) {
            //std::cout << "max: " << val << std::endl;
            return absMax;
        }
        if(val < 0) {
            //std::cout << "0: " << val << std::endl;
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
        ChangeContrast(const U absMax, const float contrast)
            : absMax_(absMax)
            , halfAbsMax_(absMax_/2.0)
            , contrast_(contrast)
        {}
        U operator()(U val) const {
            return clamp(255, ((float)val - (float)halfAbsMax_) * (float)contrast_ + (float)halfAbsMax_);
        }
    private:
        const float absMax_;
        const float halfAbsMax_;
        const float contrast_;
    };

    template <typename U>
    class ChangeGamma {
    public:
        ChangeGamma(const double gamma)
            : gamma_(gamma)
        {}
        U operator()(U val) const {
            //printf("%f\n", pow(static_cast<double>(val), gamma_));
            return clamp(255, 255.0*pow(static_cast<double>(val/255.0), gamma_));
        }
    private:
        const double gamma_;
    };
};

#endif
