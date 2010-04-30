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

OmSegment::OmSegment(OmId segmentID, SEGMENT_DATA_TYPE value, OmSegmentCache* cache)
	: mID( segmentID), mValue(value), mCache(cache)
{
	assert(cache && "must have cache in the segments");
	SetInitialColor();
	parentSegID = 0;
}

OmSegment::OmSegment(OmSegmentCache * cache) 
	: mCache(cache)
{
}

void OmSegment::Join(OmSegment * childUnknownLevel, double threshold )
{
	OmSegment * childRoot = mCache->findRoot( childUnknownLevel );

	if( GetId() == childRoot->GetId() ){
		assert(0);
	}
	
	segmentsJoinedIntoMe.insert( childRoot->GetId() );
	childRoot->setParent(this, threshold);

	mCache->addToDirtySegmentList(this);
}

void OmSegment::setParent(OmSegment * segment, double threshold)
{
	if( parentSegID ){
		assert(0);
	}

	parentSegID = segment->GetId();
	mThreshold = threshold;

	mCache->addToDirtySegmentList(this);
}

SEGMENT_DATA_TYPE OmSegment::getValue()
{
	return mValue;
}

OmIds OmSegment::getIDs()
{
	return mCache->getIDs( this );
}

SegmentDataSet OmSegment::getValues()
{
	return mCache->getValues( this );
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
	if( parentSegID ){
		return mCache->findRoot( this )->GetColor();
	}

	return mColor;
}

void OmSegment::Save()
{
	OmDataArchiveQT::ArchiveWrite( getSegmentPath(), this);

	OmId id = GetId();
	OmDataArchiveQT::ArchiveWrite(getValuePath(), &id );
}

OmHdf5Path OmSegment::getValuePath()
{
	return OmDataPaths::getSegmentValuePath( mCache->getSegmentationID(), mValue);
}

OmHdf5Path OmSegment::getSegmentPath()
{
	return OmDataPaths::getSegmentPath( mCache->getSegmentationID(), GetId() );
}

OmId OmSegment::GetId()
{
	return mID;
}

QString OmSegment::GetNote()
{
	QString customNote = mCache->getSegmentNote( GetId() );

	if( parentSegID ){
		customNote += "Parent: " + QString::number(parentSegID) + "; ";
	}

	if( !segmentsJoinedIntoMe.empty() ){
		customNote += "Children: ";
		customNote += StringHelpers::getStringFromIDset(segmentsJoinedIntoMe);
		customNote += "; ";
	}

	return customNote;
}

void OmSegment::SetNote(QString note)
{
	mCache->setSegmentNote( GetId(), note );
}

QString OmSegment::GetName()
{
	return mCache->getSegmentName( GetId() );
}

void OmSegment::SetName(QString name)
{
	mCache->setSegmentName( GetId(), name );
}

bool OmSegment::IsSelected()
{
	return mCache->isSegmentSelected( GetId() );
}

void OmSegment::SetSelected( bool isSelected )
{
	mCache->setSegmentSelected( GetId(), isSelected );
}

bool OmSegment::IsEnabled()
{
	return mCache->isSegmentEnabled( GetId() );
}

void OmSegment::SetEnabled(bool isEnabled)
{
	mCache->setSegmentEnabled( GetId(), isEnabled );
}

OmId OmSegment::getSegmentationID()
{
	return mCache->getSegmentationID();
}

void OmSegment::updateChunkCoordInfo( const OmMipChunkCoord & mipCoord )
{
	chunks.insert( mipCoord );
}

QSet< OmMipChunkCoord > & OmSegment::getChunks()
{
	return chunks;
}

OmId OmSegment::getParent()
{
	return parentSegID;
}
