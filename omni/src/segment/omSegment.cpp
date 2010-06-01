#include "omSegment.h"
#include "volume/omDrawOptions.h"
#include "common/omGl.h"
#include "system/omProjectData.h"
#include "system/omPreferences.h"
#include "system/omLocalPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "datalayer/archive/omDataArchiveQT.h"
#include "common/omDebug.h"
#include "utility/stringHelpers.h"
#include "segment/omSegmentCache.h"
#include "datalayer/omDataPaths.h"
#include "volume/omMipChunkCoord.h"
#include "datalayer/omDataPath.h"
#include "system/viewGroup/omViewGroupState.h"

OmSegment::OmSegment( const OmSegID & value, OmSegmentCache * cache)
	: mValue(value)
	, mCache(cache)
	, mParentSegID(0)
	, mImmutable(false)
	, mSize(0)
{
	SetInitialColor();
}

OmSegment::OmSegment(OmSegmentCache * cache)
	:  mCache(cache)
	, mParentSegID(0)
	, mImmutable(false)
	, mSize(0)
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

const OmSegID & OmSegment::getValue()
{
	return mValue;
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
		 (color.x * 255 < 55 && color.y * 255 < 55 && color.z * 255 < 55));

	color.x /= 2;
	color.y /= 2;
	color.z /= 2;

	mColorInt.red   = color.x * 255;
	mColorInt.green = color.y * 255;
	mColorInt.blue  = color.z * 255;
}

void OmSegment::SetColor(const Vector3 < float >& color)
{
	mColorInt.red   = color.x * 255;
	mColorInt.green = color.y * 255;
	mColorInt.blue  = color.z * 255;
	mCache->addToDirtySegmentList(this);
}

void OmSegment::ApplyColor(const OmBitfield & drawOps, OmViewGroupState * vgs, OmSegmentColorCacheType sccType)
{
	if( mParentSegID && sccType != SegmentationBreak){
		mCache->findRoot( this )->ApplyColor(drawOps, vgs, sccType);
		return;
	}

	Vector3<float> hyperColor;
	hyperColor = GetColorFloat();

	hyperColor.x *= 2.;
	hyperColor.y *= 2.;
	hyperColor.z *= 2.;

	//check coloring options
	if (drawOps & DRAWOP_SEGMENT_COLOR_HIGHLIGHT) {
		glColor3fv(OmPreferences::GetVector3f(OM_PREF_VIEW3D_HIGHLIGHT_COLOR_V3F).array);

	} else if (drawOps & DRAWOP_SEGMENT_COLOR_TRANSPARENT) {
		glColor3fva(hyperColor.array, 
			    OmPreferences::GetFloat(OM_PREF_VIEW3D_TRANSPARENT_ALPHA_FLT));

	} else if (OmLocalPreferences::getDoDiscoBall()) {
		static float s = 10.0;
		static int dir = 1;
		
		glEnable(GL_BLEND);
		glColor3fva(hyperColor.array, (s)/200+.4);
		s += .1*dir;
		if (s > 60) dir = -1;
		if (s < 10) dir = 1;
		glMaterialf(GL_FRONT, GL_SHININESS, 100-s);

	} else {
		glColor3fv(hyperColor.array);
	}
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

float OmSegment::getThreshold()
{
	return mThreshold;
}

void OmSegment::SetImmutable(bool immutable)
{
	mImmutable = immutable;
	mCache->addToDirtySegmentList(this);
}

bool OmSegment::GetImmutable()
{
	return mImmutable;
}

