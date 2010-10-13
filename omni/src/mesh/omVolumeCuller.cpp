#include "omVolumeCuller.h"
#include "common/omGl.h"
#include "common/omDebug.h"

#include <boost/make_shared.hpp>

OmVolumeCuller::OmVolumeCuller(const Matrix4f & projmodelview,
							   const NormCoord & pos,
							   const NormCoord & focus,
							   OmBitfield option)
	: mProjModelView(projmodelview)
	, mPosition(pos)
	, mFocus(focus)
	, mOptionBits(option)
{
	mFrustumCuller.setup(mProjModelView);
}

/////////////////////////////////
///////          Accessor Methods
bool OmVolumeCuller::CheckDrawOption(OmBitfield option)
{
	return mOptionBits & option;
}

OmBitfield OmVolumeCuller::GetDrawOptions()
{
	return mOptionBits;
}

const NormCoord& OmVolumeCuller::GetPosition() const
{
	return mPosition;
}

/////////////////////////////////
///////          Transform Methods

boost::shared_ptr<OmVolumeCuller>
OmVolumeCuller::GetTransformedCuller(const Matrix4f & mat,
									 const Matrix4f & matInv)
{
	return boost::make_shared<OmVolumeCuller>(mProjModelView * mat,
											  matInv * mPosition,
											  matInv * mFocus,
											  mOptionBits);
}

/////////////////////////////////
///////          Frustum Test Methods

Visibility OmVolumeCuller::TestChunk(const NormBbox& normBox)
{
	return mFrustumCuller.testAabb(normBox);
}

