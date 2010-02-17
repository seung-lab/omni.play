
#include "omVolumeCuller.h"
#include "omMipChunk.h"

#include "common/omGl.h"
#include "common/omDebug.h"

#define DEBUG 0

#pragma mark -
#pragma mark OmVolumeCuller
/////////////////////////////////
///////
///////          OmVolumeCuller
///////

OmVolumeCuller::OmVolumeCuller(const Matrix4f & projmodelview,
			       const NormCoord & pos, const NormCoord & focus, OmBitfield option)
:mProjModelView(projmodelview), mPosition(pos), mFocus(focus), mOptionBits(option)
{

	mFrustumCuller.setup(mProjModelView);
}

#pragma mark
#pragma mark Accessor Methods
/////////////////////////////////
///////          Accessor Methods
/*
const CullingOption& 
OmVolumeCuller::GetCullingOption() const {
	return mOption;
}
 */
bool OmVolumeCuller::CheckDrawOption(OmBitfield option) const
{
	return mOptionBits & option;
}

OmBitfield OmVolumeCuller::MaskDrawOptions(OmBitfield mask) const
{
	return mOptionBits & mask;
}

OmBitfield OmVolumeCuller::GetDrawOptions() const
{
	return mOptionBits;
}

const NormCoord & OmVolumeCuller::GetPosition() const
{
	return mPosition;
}

const NormCoord & OmVolumeCuller::GetFocus() const
{
	return mFocus;
}

#pragma mark
#pragma mark Transform Methods
/////////////////////////////////
///////          Transform Methods

OmVolumeCuller OmVolumeCuller::GetTransformedCuller(const Matrix4f & mat, const Matrix4f & matInv) const
{
	//compiler should optimize to alloc directly to return address
	return OmVolumeCuller(mProjModelView * mat, matInv * mPosition, matInv * mFocus, mOptionBits);
}

void OmVolumeCuller::TransformCuller(const Matrix4f & mat, const Matrix4f & matInv)
{
	//mult project-modelview mat
	mProjModelView = mProjModelView * mat;

	//inv mult position and focus
	mPosition = matInv * mPosition;
	mFocus = matInv * mFocus;
}

#pragma mark
#pragma mark Frustum Test Methods
/////////////////////////////////
///////          Frustum Test Methods

Visibility OmVolumeCuller::TestChunk(const OmMipChunk & rChunk) const
{
	return mFrustumCuller.testAabb(rChunk.GetNormExtent());
}

Visibility OmVolumeCuller::VisibilityTestNormBbox(const NormBbox & bbox) const
{
	return mFrustumCuller.testAabb(bbox);
}

/*
 *	Expensive extraction of projection and modelview matrix from OpenGL.
 *	This should only be performed once per draw loop, all else should be transformed
 *	from another culler.
 */

/*
 void
 OmVolumeCuller::Setup() {
 
 glGetFloatv(GL_PROJECTION_MATRIX, mProjection.ml);
 glGetFloatv(GL_MODELVIEW_MATRIX, mModelview.ml);
 
 bool valid;
 mPosition = mModelview.getInverse(valid).getTranslation();
 
 mFrustumCuller.setup( mProjection * mModelview);
 
 }
*/

void OmVolumeCuller::Draw()
{

	Vector3f v, r, p;
	int num_pts = 100;

	glPointSize(8.0);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POINTS);

	const Vector4f *planes = mFrustumCuller.getPlanes();

	//debug("FIXME", << "center: " << mPosition << endl;

	for (int plane_idx = 0; plane_idx < 6; ++plane_idx) {

		for (int i = 0; i < num_pts; ++i) {
			v.randomize();
			r = v - Vector3f::ONE * 0.5;
			r *= 10;
			p = planes[plane_idx].projectPointOntoPlane(mPosition + r);

			glVertex3fv(p.array);
		}

		//debug("FIXME", << "plane " << plane_idx << " : " << planes[plane_idx] << endl;

		/*
		   p = planes[plane_idx].projectPointOntoPlane( mPosition );
		   //debug("FIXME", << "plane " << plane_idx << " : " << p << endl;
		   glVertex3fv( p.array);
		 */
	}

	glEnd();

	//debug("FIXME", << mProjection << endl;
	//debug("FIXME", << mModelview << endl;
}

/*
 *	Double check matricies with manual extraction
 * http://74.125.47.132/search?q=cache:wYxUFGrcxeUJ:robertmarkmorley.com/2008/11/16/frustum-culling-in-opengl/+opengl+extract+view+frustum&hl=en&ct=clnk&cd=10&gl=us&client=safari
 */
void OmVolumeCuller::ExtractFrustum()
{
	float frustum[6][4];
	float proj[16];
	float modl[16];
	float clip[16];
	float t;

	/* Get the current PROJECTION matrix from OpenGL */
	glGetFloatv(GL_PROJECTION_MATRIX, proj);

	/* Get the current MODELVIEW matrix from OpenGL */
	glGetFloatv(GL_MODELVIEW_MATRIX, modl);

	/* Combine the two matrices (multiply projection by modelview) */
	clip[0] = modl[0] * proj[0] + modl[1] * proj[4] + modl[2] * proj[8] + modl[3] * proj[12];
	clip[1] = modl[0] * proj[1] + modl[1] * proj[5] + modl[2] * proj[9] + modl[3] * proj[13];
	clip[2] = modl[0] * proj[2] + modl[1] * proj[6] + modl[2] * proj[10] + modl[3] * proj[14];
	clip[3] = modl[0] * proj[3] + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];

	clip[4] = modl[4] * proj[0] + modl[5] * proj[4] + modl[6] * proj[8] + modl[7] * proj[12];
	clip[5] = modl[4] * proj[1] + modl[5] * proj[5] + modl[6] * proj[9] + modl[7] * proj[13];
	clip[6] = modl[4] * proj[2] + modl[5] * proj[6] + modl[6] * proj[10] + modl[7] * proj[14];
	clip[7] = modl[4] * proj[3] + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];

	clip[8] = modl[8] * proj[0] + modl[9] * proj[4] + modl[10] * proj[8] + modl[11] * proj[12];
	clip[9] = modl[8] * proj[1] + modl[9] * proj[5] + modl[10] * proj[9] + modl[11] * proj[13];
	clip[10] = modl[8] * proj[2] + modl[9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[8] * proj[3] + modl[9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];

	/* Extract the numbers for the RIGHT plane */
	frustum[0][0] = clip[3] - clip[0];
	frustum[0][1] = clip[7] - clip[4];
	frustum[0][2] = clip[11] - clip[8];
	frustum[0][3] = clip[15] - clip[12];

	/* Normalize the result */
	t = sqrt(frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2]);
	frustum[0][0] /= t;
	frustum[0][1] /= t;
	frustum[0][2] /= t;
	frustum[0][3] /= t;

	/* Extract the numbers for the LEFT plane */
	frustum[1][0] = clip[3] + clip[0];
	frustum[1][1] = clip[7] + clip[4];
	frustum[1][2] = clip[11] + clip[8];
	frustum[1][3] = clip[15] + clip[12];

	/* Normalize the result */
	t = sqrt(frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2]);
	frustum[1][0] /= t;
	frustum[1][1] /= t;
	frustum[1][2] /= t;
	frustum[1][3] /= t;

	/* Extract the BOTTOM plane */
	frustum[2][0] = clip[3] + clip[1];
	frustum[2][1] = clip[7] + clip[5];
	frustum[2][2] = clip[11] + clip[9];
	frustum[2][3] = clip[15] + clip[13];

	/* Normalize the result */
	t = sqrt(frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2]);
	frustum[2][0] /= t;
	frustum[2][1] /= t;
	frustum[2][2] /= t;
	frustum[2][3] /= t;

	/* Extract the TOP plane */
	frustum[3][0] = clip[3] - clip[1];
	frustum[3][1] = clip[7] - clip[5];
	frustum[3][2] = clip[11] - clip[9];
	frustum[3][3] = clip[15] - clip[13];

	/* Normalize the result */
	t = sqrt(frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2]);
	frustum[3][0] /= t;
	frustum[3][1] /= t;
	frustum[3][2] /= t;
	frustum[3][3] /= t;

	/* Extract the FAR plane */
	frustum[4][0] = clip[3] - clip[2];
	frustum[4][1] = clip[7] - clip[6];
	frustum[4][2] = clip[11] - clip[10];
	frustum[4][3] = clip[15] - clip[14];

	/* Normalize the result */
	t = sqrt(frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2]);
	frustum[4][0] /= t;
	frustum[4][1] /= t;
	frustum[4][2] /= t;
	frustum[4][3] /= t;

	/* Extract the NEAR plane */
	frustum[5][0] = clip[3] + clip[2];
	frustum[5][1] = clip[7] + clip[6];
	frustum[5][2] = clip[11] + clip[10];
	frustum[5][3] = clip[15] + clip[14];

	/* Normalize the result */
	t = sqrt(frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2]);
	frustum[5][0] /= t;
	frustum[5][1] /= t;
	frustum[5][2] /= t;
	frustum[5][3] /= t;

	for (int i = 0; i < 6; ++i) {
		Vector4f plane(frustum[i]);
		//debug("FIXME", << plane << endl;
	}
}
