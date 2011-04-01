#include "segment/lowLevel/omSegmentChildren.hpp"
#include "common/omDebug.h"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "segment/omSegmentIterator.h"
#include "utility/omColorUtils.hpp"

void OmSegment::setParent(OmSegment* parent, const double threshold)
{
    if(parent_){
        throw OmArgException("parent already set");
    }

    parent_ = parent;
    threshold_ = threshold;
}

/////////////////////////////////
///////         Color
void OmSegment::RandomizeColor()
{
    data_->color = OmColorUtils::GetRandomColor(data_->color);

    debugs(segmentBuild) << "final color values: "
                         << (int)data_->color.red << ","
                         << (int)data_->color.green << ","
                         << (int)data_->color.blue << "\n";
}

void OmSegment::reRandomizeColor()
{
    RandomizeColor();
}

void OmSegment::SetColor(const Vector3f& color)
{
    data_->color.red   = static_cast<uint8_t>(color.x * 255);
    data_->color.green = static_cast<uint8_t>(color.y * 255);
    data_->color.blue  = static_cast<uint8_t>(color.z * 255);
}

QString OmSegment::GetNote()
{
    QString customNote = segments_->getSegmentNote(data_->value);

    if(parent_ ){
        customNote += "Parent: "
            + QString::number(parent_->value())
            + "; ";
    }

    // if( !segmentsJoinedIntoMe_.empty() ){
    //     customNote += "Number of Children: "
    //         + QString::number( segmentsJoinedIntoMe_.size() )
    //         + "; ";
    // }

    return customNote;
}

void OmSegment::SetNote(const QString & note)
{
    segments_->setSegmentNote( data_->value, note );
}

QString OmSegment::GetName()
{
    return segments_->getSegmentName( data_->value );
}

void OmSegment::SetName(const QString & name)
{
    segments_->setSegmentName( data_->value, name );
}

bool OmSegment::IsSelected()
{
    return segments_->IsSegmentSelected( data_->value );
}

void OmSegment::SetSelected( const bool isSelected, const bool addToRecentList )
{
    segments_->setSegmentSelected( data_->value, isSelected, addToRecentList );
}

bool OmSegment::IsEnabled()
{
    return segments_->isSegmentEnabled( data_->value );
}

void OmSegment::SetEnabled( const bool isEnabled)
{
    segments_->setSegmentEnabled( data_->value, isEnabled );
}

OmID OmSegment::GetSegmentationID()
{
    return segments_->GetSegmentationID();
}

OmSegID OmSegment::getRootSegID()
{
    return segments_->findRoot(this)->data_->value;
}

const segChildCont_t& OmSegment::GetChildren()
{
    return segments_->Children()->GetChildren(this);
}
