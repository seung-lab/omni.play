#include "omSegment.h"
#include "volume/omDrawOptions.h"
#include "common/omGl.h"
#include "system/omProjectData.h"
#include "system/omPreferences.h"
#include "system/omLocalPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "utility/omDataArchiveQT.h"
#include "common/omDebug.h"
#include "utility/stringHelpers.h"
#include "segment/omSegmentCache.h"
#include "utility/omDataPaths.h"
#include "gui/toolbars/dendToolbar.h"

OmSegment::OmSegment( SEGMENT_DATA_TYPE value, OmSegmentCache* cache)
	: mValue(value), mCache(cache)
{
	assert(cache && "must have cache in the segments");
	SetInitialColor();
	parentSegID = 0;

	mParent = NULL;

	mCachedRoot = NULL;
	mCachedRootFreshness = 0;

	mCachedColor.red = 0;
	mCachedColor.green = 0;
	mCachedColor.blue = 0;
	mCachedColorFreshness = 0;
}

OmSegment::OmSegment(OmSegmentCache* cache)
	:  mCache(cache)
{
	mParent = NULL;
	mCachedRoot = NULL;
	mCachedRootFreshness = 0;
}

void OmSegment::Join(OmSegment * childUnknownLevel, double threshold )
{
	mCache->Join( this, childUnknownLevel, threshold );
}

void OmSegment::setParent(OmSegment * parent, double threshold)
{
	if( parentSegID ){
		assert(0);
	}

	parentSegID = parent->getValue();
	mParent = parent;
	mThreshold = threshold;
}

SEGMENT_DATA_TYPE OmSegment::getValue()
{
	return mValue;
}

/////////////////////////////////
///////         Color
void OmSegment::SetInitialColor()
{
	//initially random color
	do {
		mColor.randomize();
	} while ((mColor.x * 255 > 255 && mColor.y * 255 > 255 && mColor.z * 255 > 255) &&
		 (mColor.x * 255 < 55 && mColor.y * 255 < 55 && mColor.z * 255 < 55));

	mColor.x /= 2;
	mColor.y /= 2;
	mColor.z /= 2;
}

void OmSegment::SetColor(const Vector3 < float >&rColor)
{
	if( parentSegID ){
		mCache->findRoot( this )->SetColor(rColor);
		return;
	}

	mColor = rColor;
	mCache->addToDirtySegmentList(this);
}

void OmSegment::ApplyColor(const OmBitfield & drawOps)
{
	if( parentSegID ){
		mCache->findRoot( this )->ApplyColor(drawOps);
		return;
	}

	Vector3<float> hyperColor = mColor;
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
		glColor3fva(hyperColor.array, s/200+.4);
		s += .1*dir;
		if (s > 60) dir = -1;
		if (s < 10) dir = 1;
		glMaterialf(GL_FRONT, GL_SHININESS, 100-s);

	} else {
		glColor3fv(hyperColor.array);
	}
}

const Vector3 < float >& OmSegment::GetColor()
{
	if(parentSegID) {
		return mCache->findRoot( this )->GetColor();
	}

	return mColor;
}

QString OmSegment::GetNote()
{
	QString customNote = mCache->getSegmentNote( getValue() );

	if( parentSegID ){
		customNote += "Parent: " + QString::number(parentSegID) + "; ";
	}

	if( !segmentsJoinedIntoMe.empty() ){
                customNote += "Number of Children: ";
                customNote += QString::number( segmentsJoinedIntoMe.size() );
		customNote += "; ";
	}

	return customNote;
}

void OmSegment::SetNote(QString note)
{
	mCache->setSegmentNote( getValue(), note );
}

QString OmSegment::GetName()
{
	return mCache->getSegmentName( getValue() );
}

void OmSegment::SetName(QString name)
{
	mCache->setSegmentName( getValue(), name );
}

bool OmSegment::IsSelected()
{
	return mCache->isSegmentSelected( getValue() );
}

void OmSegment::SetSelected( bool isSelected )
{
	mCache->setSegmentSelected( getValue(), isSelected );
}

bool OmSegment::IsEnabled()
{
	return mCache->isSegmentEnabled( getValue() );
}

void OmSegment::SetEnabled(bool isEnabled)
{
	mCache->setSegmentEnabled( getValue(), isEnabled );
}

OmId OmSegment::getSegmentationID()
{
	return mCache->getSegmentationID();
}

OmId OmSegment::getParent()
{
	return parentSegID;
}

void OmSegment::splitChildLowestThreshold()
{
	mCache->splitChildLowestThreshold( this );
}

void OmSegment::splitTwoChildren(OmSegment * seg)
{
	mCache->splitTwoChildren(this, seg);
}

double OmSegment::getThreshold()
{
	return mThreshold;
}
