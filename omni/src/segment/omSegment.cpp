
#include "omSegment.h"

#include "volume/omDrawOptions.h"
#include "common/omGl.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omDebug.h"

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

#pragma mark
#pragma mark Accessor Methods
/////////////////////////////////
///////         Accessor Methods

const Vector3 < float >& OmSegment::GetColor() const const
{
	return mColor;
}

void OmSegment::SetColor(const Vector3 < float >&rColor)
{
	if (mJoinId) {

	}

	mColor = rColor;
}

void OmSegment::Join(OmId segid)
{
	OmId myid = GetId();

	if (myid != segid) {
		cout << segid << " is now parent id" << endl;
		mJoinId = segid;
	}
}

#pragma mark
#pragma mark Example Methods
/////////////////////////////////
///////         Example Methods

void OmSegment::ApplyColor(const OmBitfield & drawOps)
{

	//check coloring options
	if (drawOps & DRAWOP_SEGMENT_COLOR_HIGHLIGHT) {
		glColor3fv(OmPreferences::GetVector3f(OM_PREF_VIEW3D_HIGHLIGHT_COLOR_V3F).array);

	} else if (drawOps & DRAWOP_SEGMENT_COLOR_TRANSPARENT) {
		glColor3fva(mColor.array, OmPreferences::GetFloat(OM_PREF_VIEW3D_TRANSPARENT_ALPHA_FLT));

	} else {
		glColor3fv(mColor.array);
	}

}

#pragma mark
#pragma mark Print Methods
/////////////////////////////////
///////          Print Methods

void OmSegment::Print()
{
	cout << "\t\t" << mName << " (" << mId << ")" << endl;
}
