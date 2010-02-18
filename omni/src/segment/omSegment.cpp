
#include "omSegment.h"

#include "volume/omDrawOptions.h"
#include "common/omGl.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "common/omDebug.h"

#define DEBUG 0

#pragma mark -
#pragma mark OmSegment Class
/////////////////////////////////
///////
///////         OmSegment Class
///////

OmSegment::OmSegment()
{
}

OmSegment::OmSegment(OmId omId)
 : OmManageableObject(omId)
{
	original_mapped_data_value = 0;

	mName = printf2str(256, "segment%05d", omId);

	//initially random color
	do {
		mColor.randomize();
	} while ((mColor.x * 255 > 255 && mColor.y * 255 > 255 && mColor.z * 255 > 255) &&
		 (mColor.x * 255 < 55 && mColor.y * 255 < 55 && mColor.z * 255 < 55));

	mColor.x /= 2;
	mColor.y /= 2;
	mColor.z /= 2;
}

/////////////////////////////////
///////         Accessor Methods

const Vector3 < float >& OmSegment::GetColor() const
{
	return mColor;
}

void OmSegment::SetColor(const Vector3 < float >&rColor)
{
	mColor = rColor;
}

void OmSegment::set_original_mapped_data_value(const SEGMENT_DATA_TYPE value )
{
	original_mapped_data_value = value;
}


SEGMENT_DATA_TYPE OmSegment::get_original_mapped_data_value()
{
	return original_mapped_data_value;
}

/////////////////////////////////
///////         Example Methods

void OmSegment::ApplyColor(const OmBitfield & drawOps)
{
	Vector3<float> hyperColor = mColor;
	hyperColor.x *= 2.;
	hyperColor.y *= 2.;
	hyperColor.z *= 2.;

	//check coloring options
	if (drawOps & DRAWOP_SEGMENT_COLOR_HIGHLIGHT) {
		glColor3fv(OmPreferences::GetVector3f(OM_PREF_VIEW3D_HIGHLIGHT_COLOR_V3F).array);

	} else if (drawOps & DRAWOP_SEGMENT_COLOR_TRANSPARENT) {
		glColor3fva(hyperColor.array, OmPreferences::GetFloat(OM_PREF_VIEW3D_TRANSPARENT_ALPHA_FLT));

	} else {
		glColor3fv(hyperColor.array);
	}

}
