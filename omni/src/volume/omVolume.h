#ifndef OM_VOLUME_H
#define OM_VOLUME_H

/*
 *	OmVolume is the abstract represntation of multiple forms of data enclosed by
 *	a region of space.
 *
 *	Brett Warne - bwarne@mit.edu - 2/6/09
 */


#include "system/omGenericManager.h"
#include "system/omManageableObject.h"

class OmVolume : boost::noncopyable {

 public:
	//transform methods
	Vector3<float> GetScale();
	bool SetScale(const Vector3<float> &vec);
	Vector3<float> GetUserScale();
	bool SetUserScale(const Vector3<float> &vec);
	
	//data properties
	const DataBbox& GetDataExtent();
	void SetDataExtent(const DataBbox& extent);
	Vector3i GetDataDimensions();
	void SetDataDimensions(const Vector3i& dim);
	Vector3f GetDataResolution();
	bool SetDataResolution(const Vector3f& res);
	void CheckDataResolution();
	Vector2f GetStretchValues(ViewType plane);
	void SetStretchValues();
	int GetChunkDimension();
	void SetChunkDimension(int);
	QString GetUnit();
	void SetUnit(QString unit);
	
	//coordinate frame methods
	NormCoord DataToNormCoord(const DataCoord &data, bool centered = true);
	DataCoord NormToDataCoord(const NormCoord &norm);
	SpaceCoord NormToSpaceCoord(const NormCoord &norm);
	NormCoord SpaceToNormCoord(const SpaceCoord &spatial);

	DataCoord SpaceToDataCoord(const SpaceCoord &spacec);
	SpaceCoord DataToSpaceCoord(const DataCoord &datac);
	DataBbox SpaceToDataBbox(const SpaceBbox & spacebox);
	SpaceBbox DataToSpaceBbox(const DataBbox & databox);
	
	NormBbox DataToNormBbox(const DataBbox &dataBbox);
	DataBbox NormToDataBbox(const NormBbox &normBbox);
	SpaceBbox NormToSpaceBbox(const NormBbox &normBbox);
	NormBbox SpaceToNormBbox(const SpaceBbox &spacialBbox);

	const Matrix4<float> & GetNormToSpaceMatrix(){ return mNormToSpaceMat; }
	const Matrix4<float> & GetNormToSpaceInvMatrix(){ return mNormToSpaceInvMat; }

 protected:
	OmVolume();
	~OmVolume();
	
	//transforms from normailzed unit cube external spatial coverage
	Matrix4<float> mSpaceToUserMat;
	Matrix4<float> mSpaceToUserInvMat;
	Matrix4<float> mNormToSpaceMat;
	Matrix4<float> mNormToSpaceInvMat;

	//data properties
	DataBbox mDataExtent;
	Vector3f mDataResolution;	//units per voxel
	Vector3f mDataStretchValues;
	
	int mChunkDim;
	QString unitString;
	
	friend class OmDataArchiveProject;

private:
	bool Update();

};

#endif
