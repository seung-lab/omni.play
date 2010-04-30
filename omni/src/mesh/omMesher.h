#ifndef OM_MESHER_H
#define OM_MESHER_H

/*	
 *	Extracts relavent data from a MipChunk to apply the Marching Cubes algorithm
 *	to build a mesh which is stored as a MipMesh.
 *
 *	Brett Warne - bwarne@mit.edu - 3/7/09
 */

#include "omMeshSource.h"
#include "volume/omMipChunkCoord.h"


#include <vtkImageData.h>
#include <vtkWindowedSincPolyDataFilter.h>

class OmMipMesh;
class OmMipMeshManager;
class OmMipChunk;

class vtkImageData;
class vtkDiscreteMarchingCubes;
class vtkQuadricDecimation;
class vtkTransform;
class vtkTransformPolyDataFilter;
class vtkSmoothPolyDataFilter;
class vtkPolyDataNormals;
class vtkStripper;
class vtkPolyData;
class vtkCleanPolyData;
class vtkWindowedSincPolyDataFilter;

class OmMesher {
	
public:
	OmMesher( OmMeshSource & );
	~OmMesher();
	
	void InitMeshingPipeline();
	void ExtractMesh(OmMipMesh*, SEGMENT_DATA_TYPE);
	
private:
	void BuildMeshFromPolyData( vtkPolyData *pPolyData, OmMipMesh* pMesh, vtkPolyData *pPolyDataTriangles=0);
	
	//source
	OmMeshSource mMeshSource;
	
	//vtk pipeline
	vtkDiscreteMarchingCubes *mpDiscreteMarchingCubes;
	vtkQuadricDecimation *mpDecimation;
	vtkTransform *mpTransform;
	vtkTransformPolyDataFilter *mpTransformPolyDataFilter;
	vtkSmoothPolyDataFilter *mpSmoothPolyDataFilter;
	vtkPolyDataNormals *mpPolyDataNormals;
	vtkStripper *mpStripper;
	vtkCleanPolyData * mpCleanPolyData;

	bool mUseWindowedSinc;
	vtkWindowedSincPolyDataFilter *mpWindowedSincPolyDataFilter;
};

#endif
