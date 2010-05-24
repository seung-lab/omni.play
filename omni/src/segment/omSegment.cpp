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
#include "volume/omMipChunkCoord.h"
#include "utility/omHdf5Path.h"

OmSegment::OmSegment( const OmSegID & value, OmSegmentCache* cache)
	: mValue(value), mCache(cache), mParentSegID(0)
{
	assert(cache && "must have cache in the segments");
	SetInitialColor();
}

OmSegment::OmSegment(OmSegmentCache* cache)
	:  mCache(cache), mParentSegID(0)
{
}

void OmSegment::setParent(OmSegment * parent, float threshold)
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

void OmSegment::ApplyColor(const OmBitfield & drawOps)
{
	if( mParentSegID && !DendToolBar::GetSplitMode()){
		mCache->findRoot( this )->ApplyColor(drawOps);
		return;
	}

	Vector3<float> hyperColor = GetColorFloat();

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
	QString customNote = mCache->getSegmentNote( getValue() );

	if( mParentSegID ){
		customNote += "Parent: " + QString::number(mParentSegID) + "; ";
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
