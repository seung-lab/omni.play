#pragma once

/*
 *	
 */

#include "common/omStd.h"
#include "common/exception.h"




class OmCamera;

class OmCameraMovement {
public:
	OmCameraMovement(OmCamera *pCamera) : mpCamera(pCamera) { }
	
protected:
	OmCamera *mpCamera;
	Matrix4f mOldViewMatrix;
};


/*
 * Quaternion ArcBall for camera movement.
 * Based off of http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=48
 */

class OmCameraArcBall : public OmCameraMovement {

public:
	OmCameraArcBall(OmCamera *pCamera) : OmCameraMovement(pCamera) { }
	
	void Click(const Vector2<float> &point);
	void Drag(const Vector2<float> &point);
	
private:
	void MapPointToSphere(const Vector2<float> &point, Vector3<float>& vector);
	
	float mAdjustWidth, mAdjustHeight;
	Vector3<float> mStartVector, mEndVector;			//click and drag vector
	
	Quaternion<float> mStartRotation;
};





class OmCameraPan : public OmCameraMovement {
	
public:
	OmCameraPan(OmCamera *pCamera) : OmCameraMovement(pCamera) { }
	
	void Click(const Vector2<float> &point);
	void Drag(const Vector2<float> &point);
	
private:
	Vector2<float> mStartPoint;
	Vector3<float> mStartCenter;
};





class OmCameraZoom : public OmCameraMovement {

public:
	OmCameraZoom(OmCamera *pCamera) : OmCameraMovement(pCamera) { }
	
	void Click(const Vector2<float> &point);
	void Drag(const Vector2<float> &point);
	
private:
	float mStartDistance;
	Vector2<float> mStartPoint;
};


