#include "common/omDebug.h"
#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentIterator.h"
#include "utility/omRand.hpp"

OmSegment::OmSegment(const OmSegID value,
					 boost::shared_ptr<OmSegmentCache> cache)
	: value_(value)
	, mCache(cache)
	, mParentSegID(0)
	, mImmutable(false)
	, mSize(0)
	, mEdgeNumber(-1)
	, mFreshnessForMeshes(0)
{
}

void OmSegment::setParent(OmSegment * parent, const float threshold)
{
	if( mParentSegID ){
		assert(0);
	}

	mParentSegID = parent->value_;
	mThreshold = threshold;
}

/////////////////////////////////
///////         Color
void OmSegment::RandomizeColor()
{
	mColorInt = OmRand::GetRandomColor();

	debugs(segmentBuild) << "final color values: "
						 << (int)mColorInt.red << ","
						 << (int)mColorInt.green << ","
						 << (int)mColorInt.blue << "\n";
}

void OmSegment::reRandomizeColor()
{
	RandomizeColor();

	mCache->addToDirtySegmentList(this);
}

void OmSegment::SetColor(const Vector3f& color)
{
	mColorInt.red   = static_cast<quint8>(color.x * 255);
	mColorInt.green = static_cast<quint8>(color.y * 255);
	mColorInt.blue  = static_cast<quint8>(color.z * 255);

	mCache->addToDirtySegmentList(this);
}

QString OmSegment::GetNote()
{
	QString customNote = mCache->getSegmentNote(value_);

	if( mParentSegID ){
		customNote += "Parent: "
			+ QString::number(mParentSegID)
			+ "; ";
	}

	if( !segmentsJoinedIntoMe.empty() ){
		customNote += "Number of Children: "
			+ QString::number( segmentsJoinedIntoMe.size() )
			+ "; ";
	}

	return customNote;
}

void OmSegment::SetNote(const QString & note)
{
	mCache->setSegmentNote( value_, note );
}

QString OmSegment::GetName()
{
	return mCache->getSegmentName( value_ );
}

void OmSegment::SetName(const QString & name)
{
	mCache->setSegmentName( value_, name );
}

bool OmSegment::IsSelected()
{
	return mCache->IsSegmentSelected( value_ );
}

void OmSegment::SetSelected( const bool isSelected, const bool addToRecentList )
{
	mCache->setSegmentSelected( value_, isSelected, addToRecentList );
}

bool OmSegment::IsEnabled()
{
	return mCache->isSegmentEnabled( value_ );
}

void OmSegment::SetEnabled( const bool isEnabled)
{
	mCache->setSegmentEnabled( value_, isEnabled );
}

OmId OmSegment::getSegmentationID()
{
	return mCache->getSegmentationID();
}

void OmSegment::SetImmutable( const bool immutable)
{
	mImmutable = immutable;

	mCache->addToDirtySegmentList(this);
}

OmSegID OmSegment::getRootSegID()
{
	return mCache->findRoot(this)->value_;
}

quint64 OmSegment::getSizeWithChildren()
{
	return mCache->getSizeRootAndAllChildren(this);
}
