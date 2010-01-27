#ifndef OM_MIP_CHUNK_CULLER_H
#define OM_MIP_CHUNK_CULLER_H

/*
 *	A FrustumCuller wrapper specifically used to cull objects in a volume.  Rather than extracting the
 *	projection and modelview matrix for each transformed frame, the VolumeCuller can return
 *	a new transformed culler given a matrix and it's inverse (these should be precomputed).
 *	
 *	The culler also keeps track of it's position of the camera and camera's focus.
 */

#include <vmmlib/vmmlib.h>
using namespace vmml;

#include "omVolumeTypes.h"
#include "omDrawOptions.h"

#include "system/omSystemTypes.h"

class OmMipChunk;

class OmVolumeCuller {

 public:
	OmVolumeCuller(const Matrix4f & projmodelview,
		       const NormCoord & pos, const NormCoord & focus, OmBitfield options);

	//accessors
	bool CheckDrawOption(OmBitfield option) const;
	OmBitfield MaskDrawOptions(OmBitfield mask) const;
	OmBitfield GetDrawOptions() const;

	const NormCoord & GetPosition() const;
	const NormCoord & GetFocus() const;

	//transform
	OmVolumeCuller GetTransformed(const Matrix4f &, const Matrix4f &) const;
	OmVolumeCuller GetTransformedCuller(const Matrix4f &, const Matrix4f &) const;
	void TransformCuller(const Matrix4f & mat, const Matrix4f & matInv);

	//tests
	Visibility TestChunk(const OmMipChunk &) const;
	Visibility VisibilityTestNormBbox(const NormBbox &) const;

	//temp
	void Draw();
	void ExtractFrustum();

 private:
	 Matrix4f mProjModelView;
	NormCoord mPosition;
	NormCoord mFocus;
	OmBitfield mOptionBits;

	FrustumCullerf mFrustumCuller;
};

#endif
