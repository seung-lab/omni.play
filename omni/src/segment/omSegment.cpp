#include "common/omDebug.h"
#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentIterator.h"
#include "utility/omRand.hpp"

OmSegment::OmSegment(const OmSegID value,
					 boost::shared_ptr<OmSegmentCache> cache)
	: value(value)
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

	mParentSegID = parent->value;
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

void OmSegment::SetColor(const Vector3 < float >& color)
{
	mColorInt.red   = static_cast<quint8>(color.x * 255);
	mColorInt.green = static_cast<quint8>(color.y * 255);
	mColorInt.blue  = static_cast<quint8>(color.z * 255);

	mCache->addToDirtySegmentList(this);
}

QString OmSegment::GetNote()
{
	QString customNote = mCache->getSegmentNote( value );

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
	mCache->setSegmentNote( value, note );
}

QString OmSegment::GetName()
{
	return mCache->getSegmentName( value );
}

void OmSegment::SetName(const QString & name)
{
	mCache->setSegmentName( value, name );
}

bool OmSegment::IsSelected()
{
	return mCache->IsSegmentSelected( value );
}

void OmSegment::SetSelected( const bool isSelected, const bool addToRecentList )
{
	mCache->setSegmentSelected( value, isSelected, addToRecentList );
}

bool OmSegment::IsEnabled()
{
	return mCache->isSegmentEnabled( value );
}

void OmSegment::SetEnabled( const bool isEnabled)
{
	mCache->setSegmentEnabled( value, isEnabled );
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
	return mCache->findRoot(this)->value;
}

quint64 OmSegment::getSizeWithChildren()
{
	return mCache->getSizeRootAndAllChildren(this);
}
