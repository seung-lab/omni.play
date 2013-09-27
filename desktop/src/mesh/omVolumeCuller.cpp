#include "omVolumeCuller.h"
#include "common/omGl.h"
#include "common/logging.h"

OmVolumeCuller::OmVolumeCuller(const Matrix4f & projmodelview,
                            const om::normCoord & pos,
                            const om::normCoord & focus)
    : mProjModelView(projmodelview)
    , mPosition(pos)
    , mFocus(focus)
{
    mFrustumCuller.setup(mProjModelView);
}

const om::normCoord& OmVolumeCuller::GetPosition() const {
    return mPosition;
}

std::shared_ptr<OmVolumeCuller>
OmVolumeCuller::GetTransformedCuller(const Matrix4f & mat,
                                    const Matrix4f & matInv)
{
    const OmMipVolume* vol = mPosition.volume();
    return std::make_shared<OmVolumeCuller>(mProjModelView * mat,
                                           om::normCoord(matInv * mPosition, vol),
                                           om::normCoord(matInv * mFocus, vol));
}

Visibility OmVolumeCuller::TestChunk(const om::normBbox& normBox){
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
