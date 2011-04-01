#ifndef OM_SEGEMNT_COLORIZER_IMPL_HPP
#define OM_SEGEMNT_COLORIZER_IMPL_HPP

#include "segment/colorizer/omSegmentColors.hpp"
#include "segment/colorizer/omCacheSegments.hpp"
#include "segment/colorizer/omSegmentColorizerColorCache.hpp"
#include "segment/colorizer/omSegmentColorizerTypes.h"
#include "segment/omSegments.h"
#include "segment/omSegmentUtils.hpp"
#include "utility/omLockedObjects.h"
#include "viewGroup/omViewGroupState.h"

static const OmColor blackColor = {0, 0, 0};

class OmSegmentColorizerImpl {
private:
    SegmentColorParams& params_;
    OmSegmentColorizerColorCache& colorCache_;
    const uint64_t freshness_;
    const float breakThreshold_;

    bool anySegmentsSelected_;
    boost::unordered_set<OmSegID> selectedSegIDs_;
    boost::unordered_set<OmSegID> enabledSegIDs_;

    boost::unordered_map<OmSegment*, OmSegment*> rootSegMap_;
    boost::unordered_map<OmSegID, OmSegment*> segMap_;

public:
    OmSegmentColorizerImpl(SegmentColorParams& params,
                           OmSegmentColorizerColorCache& colorCache,
                           const uint64_t freshness)
        : params_(params)
        , colorCache_(colorCache)
        , freshness_(freshness)
        , breakThreshold_(params_.vgs->getBreakThreshold())
    {
        const OmSegIDsSet& selected = params.segments->GetSelectedSegmentIds();
        selectedSegIDs_ = boost::unordered_set<OmSegID>(selected.begin(),
                                                        selected.end());

        const OmSegIDsSet& enabled = params.segments->GetEnabledSegmentIds();
        enabledSegIDs_ = boost::unordered_set<OmSegID>(enabled.begin(),
                                                       enabled.end());

        anySegmentsSelected_ = !selectedSegIDs_.empty() || !enabledSegIDs_.empty();
    }

    void ColorTile(uint32_t const*const d, OmColorRGBA* colorMappedData)
    {
        OmColor prevColor = blackColor;
        OmSegID lastVal = 0;

        for(uint32_t i = 0; i < params_.numElements; ++i )
        {
            colorMappedData[i].alpha = 255;

            if( d[i] == lastVal ){ // memoized previous, non-zero color
                colorMappedData[i].red   = prevColor.red;
                colorMappedData[i].green = prevColor.green;
                colorMappedData[i].blue  = prevColor.blue;

            } else if( 0 == d[i] ){ // black
                colorMappedData[i].red   = 0;
                colorMappedData[i].green = 0;
                colorMappedData[i].blue  = 0;

            } else { // get color from cache

                uint64_t curFreshness;
                OmColor curColor;

                colorCache_.Get( d[i], curFreshness, curColor);

                if(freshness_ > curFreshness){
                    curColor = getVoxelColorForView2d( d[i] );
                    colorCache_.Set( d[i], curFreshness, curColor);
                }

                // memoize
                prevColor = curColor;
                lastVal = d[i];

                colorMappedData[i].red   = prevColor.red;
                colorMappedData[i].green = prevColor.green;
                colorMappedData[i].blue  = prevColor.blue;
            }
        }
    }

private:
    inline OmSegment* getSegment(const OmSegID val)
    {
        OmSegment* seg = segMap_[val];
        if(seg){
            return seg;
        }
        return segMap_[val] = params_.cacheSegments->GetSegment(val);
    }

    inline OmSegment* getRootSegment(OmSegment* seg)
    {
        OmSegment* rootSeg = rootSegMap_[seg];
        if(rootSeg){
            return rootSeg;
        }
        return rootSegMap_[seg] = params_.cacheSegments->GetRootSegment(seg);
    }

    inline OmColor getVoxelColorForView2d(const OmSegID val)
    {
        OmSegment* seg = getSegment(val);
        if(!seg){
            return blackColor;
        }

        return getVoxelColorForView2d(seg);
    }

    OmColor getVoxelColorForView2d(OmSegment* seg)
    {
        OmSegment* segRoot = getRootSegment(seg);

        const OmColor segRootColor = segRoot->GetColorInt();

        const bool isSelected =
            selectedSegIDs_.count(segRoot->value()) ||
            enabledSegIDs_.count(segRoot->value());

        switch(params_.sccType){
        case SCC_SEGMENTATION_VALID:
        case SCC_FILTER_VALID:
            if(seg->IsValidListType()) {
                return segRootColor;
            }
            return blackColor;

        case SCC_SEGMENTATION_VALID_BLACK:
        case SCC_FILTER_VALID_BLACK:
            if(seg->IsValidListType()) {
                return blackColor;
            }
            return segRootColor;

        case SCC_FILTER_BREAK:
        case SCC_SEGMENTATION_BREAK_BLACK:
        case SCC_SEGMENTATION_BREAK_COLOR:
            if(isSelected || !anySegmentsSelected_)
            {
                OmSegment* segToShow = seg;

                if(!qFuzzyCompare(1, breakThreshold_))
                {
                    // WARNING: operation is O(depth of MST)
                    segToShow = OmSegmentUtils::GetSegmentBasedOnThreshold(seg, breakThreshold_);
                }

                if(SCC_SEGMENTATION_BREAK_COLOR == params_.sccType){
                    return makeSelectedColor(segToShow->GetColorInt());
                }

                return segToShow->GetColorInt();
            }

            if(!isSelected && anySegmentsSelected_)
            {
                if(SCC_SEGMENTATION_BREAK_COLOR != params_.sccType){
                    return blackColor;
                }

                return makeMutedColor(seg->GetColorInt());
            }

            return seg->GetColorInt();

        default:
            if(isSelected){
                return makeSelectedColor(segRootColor);
            }

            if(SCC_FILTER_BLACK == params_.sccType &&
               anySegmentsSelected_)
            {
                return blackColor;
            }
            return segRootColor;
        }
    }

    static inline OmColor makeMutedColor(const OmColor& color)
    {
        const OmColor ret =
            {OmSegmentColors::MakeMutedColor(color.red),
             OmSegmentColors::MakeMutedColor(color.green),
             OmSegmentColors::MakeMutedColor(color.blue)
            };
        return ret;
    }

    static inline OmColor makeSelectedColor(const OmColor& color)
    {
        const OmColor ret =
            {OmSegmentColorizer::SelectedColorLookupTable[color.red],
             OmSegmentColorizer::SelectedColorLookupTable[color.green],
             OmSegmentColorizer::SelectedColorLookupTable[color.blue]
            };
        return ret;
    }
};

#endif
