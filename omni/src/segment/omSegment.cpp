#include "common/omDebug.h"
#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentIterator.h"
#include "utility/omRand.hpp"

void OmSegment::setParent(OmSegment * parent, const float threshold)
{
	if( parentSegID_ ){
		assert(0);
	}

	parentSegID_ = parent->value_;
	threshold_ = threshold;
}

/////////////////////////////////
///////         Color
void OmSegment::RandomizeColor()
{
	colorInt_ = OmRand::GetRandomColor();

	debugs(segmentBuild) << "final color values: "
						 << (int)colorInt_.red << ","
						 << (int)colorInt_.green << ","
						 << (int)colorInt_.blue << "\n";
}

void OmSegment::reRandomizeColor()
{
	RandomizeColor();

	cache_->addToDirtySegmentList(this);
}

void OmSegment::SetColor(const Vector3f& color)
{
	colorInt_.red   = static_cast<quint8>(color.x * 255);
	colorInt_.green = static_cast<quint8>(color.y * 255);
	colorInt_.blue  = static_cast<quint8>(color.z * 255);

	cache_->addToDirtySegmentList(this);
}

QString OmSegment::GetNote()
{
	QString customNote = cache_->getSegmentNote(value_);

	if( parentSegID_ ){
		customNote += "Parent: "
			+ QString::number(parentSegID_)
			+ "; ";
	}

	if( !segmentsJoinedIntoMe_.empty() ){
		customNote += "Number of Children: "
			+ QString::number( segmentsJoinedIntoMe_.size() )
			+ "; ";
	}

	return customNote;
}

void OmSegment::SetNote(const QString & note)
{
	cache_->setSegmentNote( value_, note );
}

QString OmSegment::GetName()
{
	return cache_->getSegmentName( value_ );
}

void OmSegment::SetName(const QString & name)
{
	cache_->setSegmentName( value_, name );
}

bool OmSegment::IsSelected()
{
	return cache_->IsSegmentSelected( value_ );
}

void OmSegment::SetSelected( const bool isSelected, const bool addToRecentList )
{
	cache_->setSegmentSelected( value_, isSelected, addToRecentList );
}

bool OmSegment::IsEnabled()
{
	return cache_->isSegmentEnabled( value_ );
}

void OmSegment::SetEnabled( const bool isEnabled)
{
	cache_->setSegmentEnabled( value_, isEnabled );
}

OmId OmSegment::getSegmentationID()
{
	return cache_->getSegmentationID();
}

void OmSegment::SetImmutable( const bool immutable)
{
	immutable_ = immutable;

	cache_->addToDirtySegmentList(this);
}

OmSegID OmSegment::getRootSegID()
{
	return cache_->findRoot(this)->value_;
}

quint64 OmSegment::getSizeWithChildren()
{
	return cache_->getSizeRootAndAllChildren(this);
}
