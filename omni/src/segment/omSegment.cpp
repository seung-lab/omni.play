
#include "omSegment.h"

#include "volume/omDrawOptions.h"
#include "common/omGl.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"

#define DEBUG 0



#pragma mark -
#pragma mark OmSegment Class
/////////////////////////////////
///////
///////		OmSegment Class
///////

OmSegment::OmSegment() {
}

OmSegment::OmSegment(OmId omId) 
: OmManageableObject(omId) {
	
//	mName = "segment" + i2str(omId);
	mName = printf2str(256, "segment%05d", omId);
	
	//initially random color
	mColor.randomize();
}




#pragma mark 
#pragma mark Accessor Methods
/////////////////////////////////
///////		Accessor Methods

const Vector3<float>& 
OmSegment::GetColor() const {
	return mColor;
}

void 
OmSegment::SetColor(const Vector3<float> &rColor) {
	if (mJoinId) {
		
	}

	mColor = rColor;
}



void OmSegment::Join (OmId segid)
{
	OmId myid = GetId ();

	if (myid != segid) {
		cout << segid << " is now parent id" << endl;
		mJoinId = segid;
	}
}





#pragma mark 
#pragma mark Example Methods
/////////////////////////////////
///////		Example Methods

void 
OmSegment::ApplyColor(const OmBitfield &drawOps) {
	
	//check coloring options
	if(drawOps & DRAWOP_SEGMENT_COLOR_HIGHLIGHT) {
		glColor3fv(OmPreferences::GetVector3f(OM_PREF_VIEW3D_HIGHLIGHT_COLOR_V3F).array);	
		
	} else if(drawOps & DRAWOP_SEGMENT_COLOR_TRANSPARENT) {
		glColor3fva(mColor.array, OmPreferences::GetFloat(OM_PREF_VIEW3D_TRANSPARENT_ALPHA_FLT));	
		
	} else {
		glColor3fv(mColor.array);
	}
	
}






#pragma mark 
#pragma mark Print Methods
/////////////////////////////////
///////		 Print Methods


void
OmSegment::Print() {
	cout << "\t\t" << mName << " (" << mId << ")" << endl;
}
