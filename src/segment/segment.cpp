#include "segment/lowLevel/segmentChildren.hpp"
#include "common/debug.h"
#include "segment/segment.h"
#include "segment/segments.h"
#include "segment/segmentIterator.h"
#include "utility/color.hpp"

void segment::setParent(segment* parent, const double threshold)
{
    if(parent_){
        throw OmArgException("parent already set");
    }

    parent_ = parent;
    threshold_ = threshold;
}

/////////////////////////////////
///////     Color
void segment::RandomizeColor()
{
    data_->color =  om::utils::color::GetRandomColor(data_->color);

    debugs(segmentBuild) << "final color values: "
                         << (int)data_->color.red << ","
                         << (int)data_->color.green << ","
                         << (int)data_->color.blue << "\n";
}

void segment::reRandomizeColor()
{
    RandomizeColor();
}

void segment::SetColor(const OmColor& color){
    data_->color = color;
}

void segment::SetColor(const Vector3i& color)
{
    data_->color.red  = static_cast<uint8_t>(color.x);
    data_->color.green = static_cast<uint8_t>(color.y);
    data_->color.blue = static_cast<uint8_t>(color.z);
}

void segment::SetColor(const Vector3f& color)
{
    data_->color.red  = static_cast<uint8_t>(color.x * 255);
    data_->color.green = static_cast<uint8_t>(color.y * 255);
    data_->color.blue = static_cast<uint8_t>(color.z * 255);
}

std::string segment::GetNote()
{
    std::string customNote = segments_->getSegmentNote(data_->value);

    if(parent_ ){
        customNote += "Parent: "
            + std::string::number(parent_->value())
            + "; ";
    }

    // if( !segmentsJoinedIntoMe_.empty() ){
    //   customNote += "Number of Children: "
    //     + std::string::number( segmentsJoinedIntoMe_.size() )
    //     + "; ";
    // }

    return customNote;
}

void segment::SetNote(const std::string& note){
    segments_->setSegmentNote( data_->value, note );
}

std::string segment::GetName(){
    return segments_->getSegmentName( data_->value );
}

void segment::SetName(const std::string& name){
    segments_->setSegmentName( data_->value, name );
}

bool segment::IsSelected(){
    return segments_->IsSegmentSelected( data_->value );
}

void segment::SetSelected( const bool isSelected, const bool addToRecentList ){
    segments_->setSegmentSelected( data_->value, isSelected, addToRecentList );
}

bool segment::IsEnabled(){
    return segments_->isSegmentEnabled( data_->value );
}

void segment::SetEnabled( const bool isEnabled){
    segments_->setSegmentEnabled( data_->value, isEnabled );
}

common::id segment::GetSegmentationID(){
    return segments_->GetSegmentationID();
}

segId segment::RootID(){
    return segments_->findRootID(data_->value);
}

const segChildCont_t& segment::GetChildren(){
    return segments_->Children()->GetChildren(this);
}

const coords::dataBbox segment::BoundingBox() const
{
    zi::spinlock::pool<segment_bounds_mutex_pool_tag>::guard g(data_->value);
    return coords::dataBbox(data_->bounds, segments_->getSegmentation(), 0);
}
