#include "omVolumeCuller.h"
#include "common/omGl.h"
#include "common/omDebug.h"

OmVolumeCuller::OmVolumeCuller(const Matrix4f & projmodelview,
							   const NormCoord & pos,
							   const NormCoord & focus)
	: mProjModelView(projmodelview)
	, mPosition(pos)
	, mFocus(focus)
{
	mFrustumCuller.setup(mProjModelView);
}

const NormCoord& OmVolumeCuller::GetPosition() const {
	return mPosition;
}

boost::shared_ptr<OmVolumeCuller>
OmVolumeCuller::GetTransformedCuller(const Matrix4f & mat,
									 const Matrix4f & matInv)
{
	return boost::make_shared<OmVolumeCuller>(mProjModelView * mat,
											  matInv * mPosition,
											  matInv * mFocus);
}

Visibility OmVolumeCuller::TestChunk(const NormBbox& normBox){
	return mFrustumCuller.testAabb(normBox);
}

bool matrciesAreSame(const Matrix3<float>& a, const Matrix3<float>& b)
{
    for ( size_t i = 0; i < 9; ++i ){
		if(!qFuzzyCompare(a.ml[i], b.ml[i])){
			return false;
		}
    }
    return true;
}

bool matrciesAreSame(const Matrix4<float>& a, const Matrix4<float>& b)
{
    for( size_t i = 0; i < 16; ++i ){
		if(!qFuzzyCompare(a.ml[i], b.ml[i])){
            return false;
		}
	}
    return true;
}

bool vecSame(const Vector3<float>& a, const Vector3<float>& b)
{
	return qFuzzyCompare(a.x, b.x) &&
		qFuzzyCompare(a.y, b.y) &&
		qFuzzyCompare(a.z, b.z);
}

bool OmVolumeCuller::operator ==(const OmVolumeCuller& c) const
{
	return matrciesAreSame(mProjModelView, c.mProjModelView) &&
		vecSame(mPosition, c.mPosition) &&
		vecSame(mFocus, c.mFocus);
}

bool OmVolumeCuller::operator !=(const OmVolumeCuller& c) const {
	return !(*this == c);
}
