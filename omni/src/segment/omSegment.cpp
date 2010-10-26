#include "common/omDebug.h"
#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentIterator.h"
#include "utility/omRand.hpp"

void OmSegment::setParent(OmSegment * parent, const double threshold)
{
	if( parentSegID_ ){
		assert(0);
	}

	parentSegID_ = parent->data_->value;
	threshold_ = threshold;
}

/////////////////////////////////
///////         Color
void OmSegment::RandomizeColor()
{
	data_->color = OmRand::GetRandomColor();

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
	data_->color.red   = static_cast<quint8>(color.x * 255);
	data_->color.green = static_cast<quint8>(color.y * 255);
	data_->color.blue  = static_cast<quint8>(color.z * 255);
}

QString OmSegment::GetNote()
{
	QString customNote = cache_->getSegmentNote(data_->value);

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
	cache_->setSegmentNote( data_->value, note );
}

QString OmSegment::GetName()
{
	return cache_->getSegmentName( data_->value );
}

void OmSegment::SetName(const QString & name)
{
	cache_->setSegmentName( data_->value, name );
}

bool OmSegment::IsSelected()
{
	return cache_->IsSegmentSelected( data_->value );
}

void OmSegment::SetSelected( const bool isSelected, const bool addToRecentList )
{
	cache_->setSegmentSelected( data_->value, isSelected, addToRecentList );
}

bool OmSegment::IsEnabled()
{
	return cache_->isSegmentEnabled( data_->value );
}

void OmSegment::SetEnabled( const bool isEnabled)
{
	cache_->setSegmentEnabled( data_->value, isEnabled );
}

OmID OmSegment::getSegmentationID()
{
	return cache_->getSegmentationID();
}

OmSegID OmSegment::getRootSegID()
{
	return cache_->findRoot(this)->data_->value;
}

uint64_t OmSegment::getSizeWithChildren()
{
	return cache_->getSizeRootAndAllChildren(this);
}
