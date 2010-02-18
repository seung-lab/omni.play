
#include "omMesher.h"
#include "omMipMesh.h"
#include "omMipMeshManager.h"

#include "volume/omMipChunk.h"

#include "common/omGl.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"

#include <vtkImageData.h>
#include <vtkFloatArray.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkTransform.h>

#include <vtkMarchingCubes.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkThreshold.h>
#include "common/omDebug.h"
#import <vtkCleanPolyData.h>
#import <vtkSmoothPolyDataFilter.h>
#import <vtkPolyDataNormals.h>
#import <vtkStripper.h>
#import <vtkPolyData.h>
#import <vtkTransformPolyDataFilter.h>
#import <vtkQuadricDecimation.h>

#import <vtkDecimatePro.h>

#define DEBUG 0

//utility
void srcToDestBboxTransform(const OmMeshSource & meshSource, vtkTransform * pTransform);
uint32_t numStripsInStripCellArray(vtkCellArray * pStripCellArray);

#pragma mark -
#pragma mark OmMeshSource
/////////////////////////////////
///////
///////          OmMeshSource
///////

OmMeshSource::OmMeshSource()
{
	pImageData = NULL;
}

//delete image data
OmMeshSource::~OmMeshSource()
{
	debug ("mesher1", "OmMeshSource::~OmMeshSource %i\n", pImageData);

	//delete image data if loaded
	if (pImageData != NULL)
		pImageData->Delete();
}

//load mesh data from chunk - expensive
void
 OmMeshSource::Load(shared_ptr < OmMipChunk > chunk)
{
	debug ("mesher1", "OmMeshSource::Load\n");
	MipCoord = chunk->GetCoordinate();
	SrcBbox = chunk->GetExtent();
	DstBbox = chunk->GetNormExtent();
	mChunk = chunk;
}

//copy from another mesh source - cheaper
void OmMeshSource::Copy(OmMeshSource & source)
{
	//copy values
	MipCoord = source.MipCoord;
	SrcBbox = source.SrcBbox;
	DstBbox = source.DstBbox;
	source.mChunk->Open ();
	pImageData = source.mChunk->GetMeshImageData();
	debug ("mesher1", "OmMeshSource::Copy %i\n", pImageData);
	pImageData->Update();
}

OmMeshSource& OmMeshSource::operator= (const OmMeshSource & foo)
{
	assert (0);
}

#pragma mark -
#pragma mark OmMesher
/////////////////////////////////
///////
///////          OmMesher
///////

OmMesher::OmMesher(OmMeshSource & meshSource)
{
	//debug ("mesher1", "OmMesher::OmMesher\n");
	//copy source
	mMeshSource.Copy(meshSource);

	mUseWindowedSinc = true;
	mUseWindowedSinc = sizeof (void*) == 4;
	
	//init pipeline (uses source for transform)
	InitMeshingPipeline();
}

OmMesher::~OmMesher()
{
	mpDiscreteMarchingCubes->Delete();
	mpDecimation->Delete();
	mpTransform->Delete();
	mpTransformPolyDataFilter->Delete();
	if (mUseWindowedSinc) {
		mpWindowedSincPolyDataFilter->Delete();
	} else {
		mpSmoothPolyDataFilter->Delete();
	}
	mpPolyDataNormals->Delete();
	mpStripper->Delete();
}


void
 OmMesher::InitMeshingPipeline()
{
	//create marching cubes
	mpDiscreteMarchingCubes = vtkDiscreteMarchingCubes::New();
	mpDiscreteMarchingCubes->SetInput(mMeshSource.pImageData);
	mpDiscreteMarchingCubes->GetOutput()->ReleaseDataFlagOn();

	// decimate clean poly
	mpDecimation = vtkQuadricDecimation::New();
	mpDecimation->SetInput(mpDiscreteMarchingCubes->GetOutput());
	double target_reduction = OmPreferences::GetFloat(OM_PREF_MESH_REDUCTION_PERCENT_FLT);
	//debug("FIXME", << "target_reduction: " << target_reduction << endl;
	if (mUseWindowedSinc) {
		mpDecimation->SetTargetReduction(.07);
	} else {
		mpDecimation->SetTargetReduction(target_reduction);
	}
	mpDecimation->GetOutput()->ReleaseDataFlagOn();

	//form transform to norm extent
	mpTransform = vtkTransform::New();
	srcToDestBboxTransform(mMeshSource, mpTransform);

	//perfrom transform
	mpTransformPolyDataFilter = vtkTransformPolyDataFilter::New();
	mpTransformPolyDataFilter->SetInput(mpDecimation->GetOutput());
	mpTransformPolyDataFilter->SetTransform(mpTransform);
	mpTransformPolyDataFilter->GetOutput()->ReleaseDataFlagOn();

	if (mUseWindowedSinc) {
		//sinc smooth poly
		mpWindowedSincPolyDataFilter = vtkWindowedSincPolyDataFilter::New();
		mpWindowedSincPolyDataFilter->SetInput(mpTransformPolyDataFilter->GetOutput());
		int num_smoothing_iters = OmPreferences::GetInteger(OM_PREF_MESH_NUM_SMOOTHING_ITERS_INT);
		mpWindowedSincPolyDataFilter->SetNumberOfIterations(12);	//smooth geometry
		mpWindowedSincPolyDataFilter->GetOutput()->ReleaseDataFlagOn();
		//form normals
		mpPolyDataNormals = vtkPolyDataNormals::New();
		mpPolyDataNormals->SetInputConnection(mpWindowedSincPolyDataFilter->GetOutputPort());
		//mpPolyDataNormals->SplittingOff();
		double preserved_feature_angle = OmPreferences::GetFloat(OM_PREF_MESH_PRESERVED_SHARP_ANGLE_FLT);
		mpPolyDataNormals->SetFeatureAngle(preserved_feature_angle);	//give appearance of smoothing
	} else {
		//smooth poly
		mpSmoothPolyDataFilter = vtkSmoothPolyDataFilter::New();
		mpSmoothPolyDataFilter->SetInput(mpTransformPolyDataFilter->GetOutput());
		int num_smoothing_iters = OmPreferences::GetInteger(OM_PREF_MESH_NUM_SMOOTHING_ITERS_INT);
		mpSmoothPolyDataFilter->SetNumberOfIterations(num_smoothing_iters);	//smooth geometry
		mpSmoothPolyDataFilter->GetOutput()->ReleaseDataFlagOn();
		//form normals
		mpPolyDataNormals = vtkPolyDataNormals::New();
		mpPolyDataNormals->SetInputConnection(mpSmoothPolyDataFilter->GetOutputPort());
		//mpPolyDataNormals->SplittingOff();
		double preserved_feature_angle = OmPreferences::GetFloat(OM_PREF_MESH_PRESERVED_SHARP_ANGLE_FLT);
		mpPolyDataNormals->SetFeatureAngle(preserved_feature_angle);	//give appearance of smoothing
	}

	//strip poly
	mpStripper = vtkStripper::New();
	mpStripper->SetInputConnection(mpPolyDataNormals->GetOutputPort());
	mpStripper->GetOutput()->ReleaseDataFlagOn();
}

void OmMesher::ExtractMesh(OmMipMesh * pMesh, SEGMENT_DATA_TYPE value)
{

	//update meshing value
	mpDiscreteMarchingCubes->SetValue(0, value);
	//mpDecimation->Update();

	//get poly from stripper
	vtkPolyData *p_poly_data = mpStripper->GetOutput();
	p_poly_data->Squeeze();
	p_poly_data->Update();

	//make mesh from poly data
	BuildMeshFromPolyData(p_poly_data, pMesh);
}

void OmMesher::BuildMeshFromPolyData(vtkPolyData * pPolyData, OmMipMesh * pMesh)
{

	//check for polyData that has been decimated to nothing
	if (pPolyData->GetNumberOfCells() == 0)
		return;

	//convert points and norms from polyData into float arrays (allow for quick copy later)
	vtkFloatArray *p_points_data_array = vtkFloatArray::New();	//polyData->GetPoints()->GetData();
	p_points_data_array->DeepCopy(pPolyData->GetPoints()->GetData());

	vtkFloatArray *p_normal_data_array = vtkFloatArray::New();
	p_normal_data_array->DeepCopy(pPolyData->GetPointData()->GetNormals());
	//debug("genone","OmMipChunkMesher::BuildMeshFromPolyData: convert to floats");

	//get strip cells properties
	vtkCellArray *p_strip_cells_array = pPolyData->GetStrips();
	uint32_t num_strips = numStripsInStripCellArray(p_strip_cells_array);
	uint32_t strip_cells_array_size = p_strip_cells_array->GetNumberOfConnectivityEntries();
	vtkIdType *p_strip_cells_array_data = p_strip_cells_array->GetPointer();
	//debug("genone","OmMipChunkMesher::BuildMeshFromPolyData: get strip props");

	//create aliases to mesh data
	uint32_t *&r_mesh_strip_offset_size_data = pMesh->mpStripOffsetSizeData;
	GLuint *&r_mesh_vert_indicies = pMesh->mpVertexIndexData;
	GLfloat *&r_mesh_vert_data = pMesh->mpVertexData;
	//debug("genone","OmMipChunkMesher::BuildMeshFromPolyData: mesh alias");

	//setup and alloc mesh memory
	pMesh->mStripCount = num_strips;
	r_mesh_strip_offset_size_data = new uint32_t[2 * num_strips];

	//strip_cells_array_size - num_strips = num verticies in strip_cell_array
	pMesh->mVertexIndexCount = strip_cells_array_size - num_strips;
	r_mesh_vert_indicies = new GLuint[pMesh->mVertexIndexCount];

	//allocate 2 (pos/norm) * 3 (x/y/z) * number of verticies
	uint32_t vert_count = p_points_data_array->GetNumberOfTuples();
	pMesh->mVertexCount = vert_count;
	r_mesh_vert_data = new GLfloat[6 * vert_count];
	//debug("genone","OmMipChunkMesher:: allocd mesh memory");

	//copy interleved position and normal data
	for (uint32_t idx = 0; idx < vert_count; idx++) {
		p_points_data_array->GetTupleValue(idx, &r_mesh_vert_data[6 * idx]);
		p_normal_data_array->GetTupleValue(idx, &r_mesh_vert_data[6 * idx + 3]);
	}
	//debug("genone","OmMipChunkMesher:: got vertex data");

	//store strip geom index offset and geom size data and geom indicies
	uint32_t strip_idx = 0;
	uint32_t mesh_vert_indicies_idx = 0;
	for (uint32_t idx = 0; idx < strip_cells_array_size;) {

		//store offset to first index in the strip
		r_mesh_strip_offset_size_data[2 * strip_idx] = mesh_vert_indicies_idx;
		//store strip size
		vtkIdType strip_geom_size = p_strip_cells_array_data[idx];
		r_mesh_strip_offset_size_data[2 * strip_idx + 1] = strip_geom_size;

		//adv index to strip data
		idx++;

		//for each geom in the strip
		for (int strip_geom_cnt = 0; strip_geom_cnt < strip_geom_size; strip_geom_cnt++, mesh_vert_indicies_idx++, idx++)	//for each element, adv all
		{
			//store geom index data
			r_mesh_vert_indicies[mesh_vert_indicies_idx] = p_strip_cells_array_data[idx];
		}

		//adv to next strip
		strip_idx++;
	}

	debug("mesher1","OmMipChunkMesher:: strips ref: %i \n", p_strip_cells_array->GetReferenceCount());
	//p_strip_cells_array->Delete();
	p_points_data_array->Delete();
	p_normal_data_array->Delete();
}

#pragma mark
#pragma mark Utility Methods
/////////////////////////////////
///////          Utility Methods

/*
 *	Returns number of strips found in a given vtkCellArray
 *	Strip cell array in format:
 *	[ n1 v1 v2 v3 v4   n2 v1 v2 v3 v4  .... ]
 *	for each strip, number of verts n1 followed by vertex indicies.
 */
uint32_t numStripsInStripCellArray(vtkCellArray * pStripCellArray)
{
	//access strip cell array
	vtkIdType *p_strip_cell_array_data = pStripCellArray->GetPointer();
	uint32_t strip_cell_array_size = pStripCellArray->GetNumberOfConnectivityEntries();

	uint32_t strip_cnt = 0;
	uint32_t idx = 0;

	//walk cell array by strips
	while (idx < strip_cell_array_size) {
		//inc index by num pts + 1
		idx += (p_strip_cell_array_data[idx] + 1);
		strip_cnt++;
	}

	return strip_cnt;
}

void srcToDestBboxTransform(const OmMeshSource & meshSource, vtkTransform * pTransform)
{
	Vector3 < float >src_dim = meshSource.SrcBbox.getMax() - meshSource.SrcBbox.getMin();	//voxels
	src_dim += Vector3 < float >::ONE;
	Vector3 < float >dst_dim = meshSource.DstBbox.getMax() - meshSource.DstBbox.getMin();	//norm coord
	Vector3 < float >ratio = dst_dim / src_dim;

	pTransform->Identity();
	pTransform->Translate(meshSource.DstBbox.getMin().array);
	pTransform->Scale((dst_dim / src_dim).array);

	pTransform->Update();
}
