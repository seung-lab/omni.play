#include "omCamera.h"
#include "common/omGl.h"
#include "common/omDebug.h"

static const float INITIAL_DISTANCE = 20;
static const Vector3f INITIAL_DIRECTION(1, 0, 0);	//direction
static const Vector3f INITIAL_FOCUS(0, 0, 0);	//focus
static const Vector3f INITIAL_UP(0, 1, 0);     //orientation

OmCamera::OmCamera()
	: mZoomer(this)
	, mPanner(this)
	, mArcBall(this)
{

	//initialize view matrix
	mModelViewMatrix = Matrix4 < float >::IDENTITY;
	mLookAtMatrix = Matrix4 < float >::IDENTITY;
	mZoomMatrix = Matrix4 < float >::IDENTITY;
	mOrbitMatrix = Matrix4 < float >::IDENTITY;
	mFocusMatrix = Matrix4 < float >::IDENTITY;

	//for arcball
	//SetFocus(INITIAL_FOCUS);
	//SetDistance(INITIAL_POSITION.length());
}

/////////////////////////////////
///////          State Accessors

/*
 * Specifies the affine transformation of x and y from
 * normalized device coordinates to	window coordinates
 */
Vector4 < int > OmCamera::GetViewport() const
{
	return mViewport;
}

void OmCamera::SetViewport(const Vector4 < int >&view)
{
	mViewport = view;
	ApplyViewport();
}

void OmCamera::ApplyViewport()
{
	glViewport(mViewport.lowerLeftX, mViewport.lowerLeftY, mViewport.width, mViewport.height);
}

/*
 *	Set the camera perspective
 */
Vector4 < float > OmCamera::GetPerspective() const
{
	return mPerspective;
}

void OmCamera::SetPerspective(const Vector4 < int >&persp)
{
	mPerspective = persp;
	ApplyPerspective();
}

/*
 * Apply camera perspective to GL state.
 */
void OmCamera::ApplyPerspective()
{
	//setup transform from eye space coordinates into clip coordinates
	//http://www.opengl.org/resources/faq/technical/viewing.htm

	//perspective changes should be the only transforms applied to the projection matrix
	glMatrixMode(GL_PROJECTION);

	//refresh projection matrix
	glLoadIdentity();
	gluPerspective(mPerspective[0], mPerspective[1], mPerspective[2], 10000000);	//glFrustum()

	//store projection matrix
	glGetFloatv(GL_PROJECTION_MATRIX, mProjectionMatrix.ml);

	//back to modelview matrix for geometry
	glMatrixMode(GL_MODELVIEW);
}

/*
 * Apply window dimension change.
 */
void OmCamera::ApplyReshape(const Vector2 < int >&dims)
{
	//update viewport
	mViewport.width = dims.x;
	mViewport.height = dims.y;

	//update aspect ratio in perspective
	mPerspective[1] = float (dims.x) / dims.y;

	//apply changes;
	ApplyViewport();
	ApplyPerspective();
}

/*
 * Resets to initial modelview matrix.  First GL call to be made.
 */

void OmCamera::ResetModelview()
{
	///use gluLookAt to compute initial orbit
	//set to modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//compute rotation matrix as if at zero looking at initial direction
	gluLookAt(0, 0, 0,
		  INITIAL_DIRECTION.x, INITIAL_DIRECTION.y, INITIAL_DIRECTION.z,
		  INITIAL_UP.x, INITIAL_UP.y, INITIAL_UP.z);

	//set orbit rotation from lookat
	glGetFloatv(GL_MODELVIEW_MATRIX, mOrbitMatrix.ml);

	//extract rotation
	Matrix3f orbit_rotation_matrix;
	mOrbitMatrix.get3x3SubMatrix(orbit_rotation_matrix);
	SetOrbitRotation(Quaternionf(orbit_rotation_matrix));

	//set lookat to be head on
	SetLookAtRotation(Quaternion < float >::IDENTITY);

	//set initial focus and distance
	SetFocus(INITIAL_FOCUS);
	SetDistance(INITIAL_DISTANCE);

	//update with reset values
	UpdateModelview();
}

void OmCamera::UpdateModelview()
{
	mModelViewMatrix = mLookAtMatrix * mZoomMatrix * mOrbitMatrix * mFocusMatrix;
}

/*
 * Apply ModelView to GL (every draw call).
 */
void OmCamera::ApplyModelview()
{

	//load model view matrix into GL
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(mModelViewMatrix.ml);
}

/////////////////////////////////
///////
///////          Property Accessors
///////

Vector3f OmCamera::GetPosition()
{
	bool valid;
	return mModelViewMatrix.getInverse(valid).getTranslation();
}

Vector3f OmCamera::GetFocus()
{
	return -mFocusMatrix.getTranslation();
}

void OmCamera::SetFocus(const Vector3f & focus)
{
	mFocusMatrix.setTranslation(-focus);
	UpdateModelview();
}

float OmCamera::GetDistance()
{
	return -mZoomMatrix.ml[14];
}

void OmCamera::SetDistance(float distance)
{
	//z-component of translation vector
	mZoomMatrix.ml[14] = -distance;
	UpdateModelview();
}

Quaternionf OmCamera::GetOrbitRotation()
{
	return mOrbitRotation;
}

void OmCamera::SetOrbitRotation(const Quaternionf & rRotation)
{
	mOrbitRotation = rRotation;
	mOrbitMatrix.set3x3SubMatrix(mOrbitRotation.getRotationMatrix(), 0, 0);
	UpdateModelview();
}

Quaternionf OmCamera::GetLookAtRotation()
{
	return mLookAtRotation;
}

void OmCamera::SetLookAtRotation(const Quaternionf & rRotation)
{
	mLookAtRotation = rRotation;
	mLookAtMatrix.set3x3SubMatrix(mLookAtRotation.getRotationMatrix(), 0, 0);
	UpdateModelview();
}

const Matrix4 < float >& OmCamera::GetModelViewMatrix() const
{
	return mModelViewMatrix;
}

const Matrix4 < float >& OmCamera::GetProjectionMatrix() const
{
	return mProjectionMatrix;
}

Matrix4 < float > OmCamera::GetProjModelViewMatrix() const
{
	return mProjectionMatrix * mModelViewMatrix;
}

/////////////////////////////////
///////          Draw

void OmCamera::DrawFocusAxis()
{
	//store
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	//draw at focus point
	glLoadIdentity();
	glLoadMatrixf((mLookAtMatrix * mZoomMatrix * mOrbitMatrix).ml);

	//scale to maintain axis size
	const float scale = GetDistance() / 30.0f;
	glScalef(scale, scale, scale);

	//draw axis
	glDrawAxis(2);

	//restore original modelview
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

/////////////////////////////////
///////          Movement

void OmCamera::MovementStart(CameraMovementType type, const Vector2 < float >&point)
{
	mMovementType = type;

	switch (type) {
	case CAMERA_PAN:
		mPanner.Click(point);
		break;
	case CAMERA_ZOOM:
		mZoomer.Click(point);
		break;
	case CAMERA_LOOKAT_ROTATE:
		mArcBall.Click(point);
		break;
	case CAMERA_ORBIT_ROTATE:
		mArcBall.Click(point);
		break;
	case CAMERA_NONE:
		break;
	}
}

void OmCamera::MovementUpdate(const Vector2 < float >&point)
{
	switch (mMovementType) {
	case CAMERA_PAN:
		mPanner.Drag(point);
		break;
	case CAMERA_ZOOM:
		mZoomer.Drag(point);
		break;
	case CAMERA_LOOKAT_ROTATE:
		mArcBall.Drag(point);
		break;
	case CAMERA_ORBIT_ROTATE:
		mArcBall.Drag(point);
		break;
	case CAMERA_NONE:
		break;
	}
}

void OmCamera::MovementEnd(const Vector2 < float >& )
{
	mMovementType = CAMERA_NONE;
}
