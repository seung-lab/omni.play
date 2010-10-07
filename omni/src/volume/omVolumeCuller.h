#ifndef OM_VOLUME_CULLER_H
#define OM_VOLUME_CULLER_H

/*
 *	A FrustumCuller wrapper specifically used to cull objects in a volume.  Rather than extracting the
 *	projection and modelview matrix for each transformed frame, the VolumeCuller can return
 *	a new transformed culler given a matrix and it's inverse (these should be precomputed).
 *
 *	The culler also keeps track of it's position of the camera and camera's focus.
 */

#include "common/omCommon.h"
#include "omDrawOptions.h"

class OmMipChunk;

class OmVolumeCuller {

public:
	OmVolumeCuller(const Matrix4f& projmodelview,
				   const NormCoord& pos,
				   const NormCoord& focus,
				   OmBitfield options);

	Visibility TestChunk( OmMipChunk &);
	bool CheckDrawOption(OmBitfield option);
	OmBitfield GetDrawOptions();
	NormCoord& GetPosition();

	boost::shared_ptr<OmVolumeCuller> GetTransformedCuller(const Matrix4f&,
														   const Matrix4f&);

private:
	Matrix4f mProjModelView;
	NormCoord mPosition;
	NormCoord mFocus;
	OmBitfield mOptionBits;

	FrustumCullerf mFrustumCuller;
};

#endif
