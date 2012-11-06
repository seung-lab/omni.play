#pragma once

#include "segment/colorizer/omSegmentColors.hpp"
#include "segment/colorizer/omSegmentColorizerColorCache.hpp"
#include "segment/colorizer/omSegmentColorizerTypes.h"
#include "segment/omSegments.h"
#include "segment/omSegmentUtils.hpp"
#include "utility/omLockedObjects.h"
#include "viewGroup/omViewGroupState.h"

static const om::common::Color blackColor = {0, 0, 0};

class OmSegmentColorizerImpl {
private:
    SegmentColorParams& params_;

    OmSegments *const segments_;

    OmSegmentColorizerColorCache& colorCache_;
    const uint64_t freshness_;
    const float breakThreshold_;

    bool anySegmentsSelected_;
    boost::unordered_set<om::common::SegID> selectedSegIDs_;
    boost::unordered_set<om::common::SegID> enabledSegIDs_;

public:
    OmSegmentColorizerImpl(SegmentColorParams& params,
                           OmSegmentColorizerColorCache& colorCache,
                           const uint64_t freshness)
        : params_(params)
        , segments_(params_.segments)
        , colorCache_(colorCache)
        , freshness_(freshness)
        , breakThreshold_(params_.vgs->getBreakThreshold())
    {
        const om::common::SegIDSet selected = params.segments->GetSelectedSegmentIDs();
        selectedSegIDs_ = boost::unordered_set<om::common::SegID>(selected.begin(),
                                                        selected.end());

        const om::common::SegIDSet enabled = params.segments->GetEnabledSegmentIDs();
        enabledSegIDs_ = boost::unordered_set<om::common::SegID>(enabled.begin(),
                                                       enabled.end());

        anySegmentsSelected_ = !selectedSegIDs_.empty() || !enabledSegIDs_.empty();
    }

    void ColorTile(uint32_t const*const d, om::common::ColorARGB* colorMappedData)
    {
        PrevSegAndColor prev = { 0, blackColor };
        uint32_t maxSize = colorCache_.Size();

        for(uint32_t i = 0; i < params_.numElements; ++i )
        {
            colorMappedData[i].alpha = 255; // required for Mac OS X?

            if( d[i] == prev.segID )
            { // use memoized previous, non-zero color
                colorMappedData[i].red   = prev.color.red;
                colorMappedData[i].green = prev.color.green;
                colorMappedData[i].blue  = prev.color.blue;

            } else if( 0 == d[i] || d[i] > maxSize)
            { // black
                colorMappedData[i].red   = 0;
                colorMappedData[i].green = 0;
                colorMappedData[i].blue  = 0;

            } else
            { // get color from cache

                uint64_t curFreshness;

                colorCache_.Get( d[i], curFreshness, prev.color);

                if(freshness_ > curFreshness)
                { // figure out color and update cache
                    prev.color = getVoxelColorForView2d( d[i] );
                    colorCache_.Set( d[i], curFreshness, prev.color);
                }

                // memoize
                prev.segID = d[i];

                colorMappedData[i].red   = prev.color.red;
                colorMappedData[i].green = prev.color.green;
                colorMappedData[i].blue  = prev.color.blue;
            }
        }
    }

private:
    struct PrevSegAndColor {
        om::common::SegID segID;
        om::common::Color color;
    };

    inline OmSegment* getSegment(const om::common::SegID segID)
    {
        // return segments_->GetSegmentUnsafe(segID);
        return segments_->GetSegment(segID);
    }

    om::common::Color getVoxelColorForView2d(const om::common::SegID segID)
    {
        //OmSegment* seg = getSegment(segID);
        OmSegment* seg = getSegment(segID);

        if(!seg) {
        	return blackColor;
        }

        OmSegment* segRoot = seg;
        if(segRoot->getParent()){
            segRoot = getSegment(segments_->findRootID(segID));
        }

        const om::common::Color segRootColor = segRoot->GetColorInt();

        const bool isSelected =
            selectedSegIDs_.count(segRoot->value()) ||
            enabledSegIDs_.count(segRoot->value());

        switch(params_.sccType)
        {
        case om::common::om::common::SCC_SEGMENTATION_VALID:
        case om::common::SCC_FILTER_VALID:
            if(seg->IsValidListType()) {
                return segRootColor;
            }
            return blackColor;

        case om::common::om::common::om::common::SCC_SEGMENTATION_VALID_BLACK:
        case om::common::om::common::SCC_FILTER_VALID_BLACK:
            if(seg->IsValidListType()) {
                return blackColor;
            }
            return segRootColor;

        case om::common::SCC_FILTER_BREAK:
        case om::common::SCC_SEGMENTATION_BREAK_BLACK:
        case om::common::om::common::SCC_SEGMENTATION_BREAK_COLOR:
            if(isSelected || !anySegmentsSelected_)
            {
                OmSegment* segToShow = seg;

                if(!qFuzzyCompare(1, breakThreshold_))
                {
                    // WARNING: operation is O(depth of MST)
                    segToShow = OmSegmentUtils::GetSegmentBasedOnThreshold(seg, breakThreshold_);
                }

                if(om::common::om::common::SCC_SEGMENTATION_BREAK_COLOR == params_.sccType){
                    return makeSelectedColor(segToShow->GetColorInt());
                }

                return segToShow->GetColorInt();
            }

            if(!isSelected && anySegmentsSelected_)
            {
                if(om::common::om::common::SCC_SEGMENTATION_BREAK_COLOR != params_.sccType){
                    return blackColor;
                }

                return makeMutedColor(seg->GetColorInt());
            }

            return seg->GetColorInt();

        default:
            if(isSelected)
            {
                if(om::common::SCC_FILTER_BLACK_DONT_BRIGHTEN_SELECT == params_.sccType ||
                   om::common::SCC_FILTER_COLOR_DONT_BRIGHTEN_SELECT == params_.sccType)
                {
                    return segRootColor;
                }

                return makeSelectedColor(segRootColor);
            }

            if(om::common::SCC_FILTER_BLACK_BRIGHTEN_SELECT == params_.sccType ||
               om::common::SCC_FILTER_BLACK_DONT_BRIGHTEN_SELECT == params_.sccType)
            {
                if(anySegmentsSelected_){
                    return blackColor;
                }
            }

            return segRootColor;
        }
    }

    // TODO: might be faster to compute: lookup could affect cache lines
    static inline om::common::Color makeMutedColor(const om::common::Color& color)
    {
        const om::common::Color ret =
            {OmSegmentColors::MakeMutedColor(color.red),
             OmSegmentColors::MakeMutedColor(color.green),
             OmSegmentColors::MakeMutedColor(color.blue)
            };
        return ret;
    }

    // TODO: might be faster to compute: lookup could affect cache lines
    static inline om::common::Color makeSelectedColor(const om::common::Color& color)
    {
        const om::common::Color ret =
            {OmSegmentColorizer::SelectedColorLookupTable[color.red],
             OmSegmentColorizer::SelectedColorLookupTable[color.green],
             OmSegmentColorizer::SelectedColorLookupTable[color.blue]
            };
        return ret;
    }
};

