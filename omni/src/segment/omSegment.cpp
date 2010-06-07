#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"

OmSegment::OmSegment( const OmSegID value, OmSegmentCache * cache)
	: mValue(value)
	, mCache(cache)
	, mParentSegID(0)
	, mImmutable(false)
	, mSize(0)
	, mSizeOfAllChildren(0)
	, mEdgeNumber(-1)
	, mCustomMergeEdge(NULL)
{
	SetInitialColor();
}

OmSegment::OmSegment(OmSegmentCache * cache)
	: mValue(0)
	, mCache(cache)
	, mParentSegID(0)
	, mImmutable(false)
	, mSize(0)
	, mSizeOfAllChildren(0)
	, mEdgeNumber(-1)
	, mCustomMergeEdge(NULL)
{
}

void OmSegment::setParent(OmSegment * parent, const float threshold)
{
	if( mParentSegID ){
		assert(0);
	}

	mParentSegID = parent->mValue;
	mThreshold = threshold;
}

/////////////////////////////////
///////         Color
void OmSegment::SetInitialColor()
{
	Vector3<float> color;

	//initially random color
	do {
		color.randomize();
	} while ((color.x * 255 > 255 && color.y * 255 > 255 && color.z * 255 > 255) &&
		 (color.x * 255 < 85 && color.y * 255 < 85 && color.z * 255 < 85));

	color.x /= 2;
	color.y /= 2;
	color.z /= 2;

	mColorInt.red   = static_cast<quint8>(color.x * 255);
	mColorInt.green = static_cast<quint8>(color.y * 255);
	mColorInt.blue  = static_cast<quint8>(color.z * 255);
}

void OmSegment::reRandomizeColor()
{
	SetInitialColor();
	
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
	QString customNote = mCache->getSegmentNote( mValue );

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
	mCache->setSegmentNote( mValue, note );
}

QString OmSegment::GetName()
{
	return mCache->getSegmentName( mValue );
}

void OmSegment::SetName(const QString & name)
{
	mCache->setSegmentName( mValue, name );
}

bool OmSegment::IsSelected()
{
	return mCache->isSegmentSelected( mValue );
}

void OmSegment::SetSelected( const bool isSelected )
{
	mCache->setSegmentSelected( mValue, isSelected );
}

bool OmSegment::IsEnabled()
{
	return mCache->isSegmentEnabled( mValue );
}

void OmSegment::SetEnabled( const bool isEnabled)
{
	mCache->setSegmentEnabled( mValue, isEnabled );
}

OmId OmSegment::getSegmentationID()
{
	return mCache->getSegmentationID();
}

void OmSegment::splitChildLowestThreshold()
{
	mCache->splitChildLowestThreshold( this );
}

void OmSegment::splitTwoChildren(OmSegment * seg)
{
	mCache->splitTwoChildren(this, seg);
}

void OmSegment::SetImmutable( const bool immutable)
{
	mImmutable = immutable;

	mCache->addToDirtySegmentList(this);
}

quint64 OmSegment::getSizeWithChlidren()
{
	return mSize + mSizeOfAllChildren;
}

OmSegID OmSegment::getRootSegID()
{
	return mCache->findRoot(this)->getValue();
}

