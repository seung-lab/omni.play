#ifndef OM_MESHER_H
#define OM_MESHER_H

/*	
 *	Extracts relavent data from a MipChunk to apply the Marching Cubes algorithm
 *	to build a mesh which is stored as a MipMesh.
 *
 *	Brett Warne - bwarne@mit.edu - 3/7/09
 */

#include "volume/omMipChunkCoord.h"
#include "segment/omSegmentTypes.h"
#include "system/omSystemTypes.h"
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <vmmlib/vmmlib.h>
using namespace vmml;


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
class vtkWindowedSincPolyDataFilter;





class OmMeshSource {
	
public:
	OmMeshSource();
	~OmMeshSource();
	
	//load mesh data from chunk - expensive
	void Load( shared_ptr<OmMipChunk> chunk );
	
	//copy from another mesh source - cheaper
	void Copy( OmMeshSource &source );
	
	vtkImageData *pImageData;
	OmMipChunkCoord MipCoord;
	AxisAlignedBoundingBox<int> SrcBbox;
	AxisAlignedBoundingBox<float> DstBbox;
	
protected:
        shared_ptr<OmMipChunk> mChunk;
	OmMeshSource(OmMeshSource &);
	OmMeshSource& operator= (const OmMeshSource &);
};





class OmMesher {
	
public:
	OmMesher( OmMeshSource & );
	~OmMesher();
	
	void InitMeshingPipeline();
	void ExtractMesh(OmMipMesh*, SEGMENT_DATA_TYPE);
	
private:
	void BuildMeshFromPolyData( vtkPolyData *pPolyData, OmMipMesh* pMesh);
	
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

	bool mUseWindowedSinc;
	vtkWindowedSincPolyDataFilter *mpWindowedSincPolyDataFilter;
};



#endif
