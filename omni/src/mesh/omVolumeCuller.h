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

class OmVolumeCuller {
public:
	OmVolumeCuller(const Matrix4f& projmodelview,
				   const NormCoord& pos,
				   const NormCoord& focus);

	Visibility TestChunk(const NormBbox&);
	const NormCoord& GetPosition() const;

	boost::shared_ptr<OmVolumeCuller>
	GetTransformedCuller(const Matrix4f&, const Matrix4f&);

	bool operator ==(const OmVolumeCuller& c) const;
	bool operator !=(const OmVolumeCuller& c) const;
	bool equals(const boost::shared_ptr<OmVolumeCuller>& c){
		return *this == *c;
	}

private:
	const Matrix4f mProjModelView;
	const NormCoord mPosition;
	const NormCoord mFocus;

	FrustumCullerf mFrustumCuller;
};

#endif
