#include "volume/segmentationDataWrapper.hpp"
#include "segment/lowLevel/segmentsImplLowLevel.h"
#include "volume/segmentation.h"

namespace om {
namespace segment {

segmentsImplLowLevel::segmentsImplLowLevel(volume::segmentation* segmentation,
                                           segmentsStore* segmentPages)
    : segmentation_(segmentation)
    , store_(segmentPages)
    , mNumSegs(0)
{
    maxValue_.set(0);
}

segmentsImplLowLevel::~segmentsImplLowLevel()
{}

std::string segmentsImplLowLevel::getSegmentName(common::segId segID)
{
    if(segmentCustomNames.empty()){
        return "";
    }

    map_it it = segmentCustomNames.find(segID);

    if(it != segmentCustomNames.end()){
        return it->second;
    }

    return ""; //std::string("segment%1").arg(segID);
}

void segmentsImplLowLevel::setSegmentName(common::segId segID, std::string name){
    segmentCustomNames[ segID ] = name;
}

std::string segmentsImplLowLevel::getSegmentNote(common::segId segID)
{
    if(segmentNotes.empty()){
        return "";
    }

    map_it it = segmentCustomNames.find(segID);

    if(it != segmentCustomNames.end()){
        return it->second;
    }

    return "";
}

void segmentsImplLowLevel::setSegmentNote(common::segId segID, std::string note){
    segmentNotes[ segID ] = note;
}

volume::SegmentationDataWrapper segmentsImplLowLevel::GetSDW() const {
    return segmentation_->GetSDW();
}

} // namespace segment
} // namespace om
