#include "common/omDebug.h"
#include "common/omException.h"
#include "common/omVtk.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/hdf5/omHdf5.h"
#include "utility/omImageDataIo.h"

#include <QFile>
#include <QFileInfo>
#include <QImage>

#include <vtk_tiff.h>
#include <vtkTIFFReader.h>
#include <vtkJPEGReader.h>
#include <vtkPNGReader.h>
#include <vtkImageReader2.h>
#include <vtkTIFFWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPNGWriter.h>
#include <vtkImageWriter.h>
#include <vtkImageData.h>
#include <vtkExtractVOI.h>
#include <vtkImageConstantPad.h>
#include <vtkImageFlip.h>
#include <vtkImageTranslateExtent.h>
#include <vtkImageAppend.h>
#include <vtkImageCast.h>
#include <vtkStringArray.h>

OmDataWrapperPtr
OmImageDataIo::om_imagedata_read_hdf5( QFileInfoList sourceFilenamesAndPaths,
				       const DataBbox dataExtentBbox,
				       const OmDataPath dataset)
{
	//FIXME: don't assert, or check before calling me!
	assert((sourceFilenamesAndPaths.size() == 1) && "More than one hdf5 file specified.h");

	OmDataLayer dl;
	OmDataReader * hdf5reader =
		dl.getReader( sourceFilenamesAndPaths[0].filePath(), true );
	hdf5reader->open();

	OmDataWrapperPtr data;
        if(hdf5reader->dataset_exists(dataset)){
		data = hdf5reader->dataset_image_read_trim( dataset, dataExtentBbox);
	} else {
		data = hdf5reader->dataset_image_read_trim( OmDataPaths::getDefaultDatasetName(), dataExtentBbox);
	}

	hdf5reader->close();

	return data;
}

Vector3 < int > OmImageDataIo::om_imagedata_get_dims_hdf5( QFileInfoList sourceFilenamesAndPaths, const OmDataPath dataset )
{
	assert((sourceFilenamesAndPaths.size() == 1) && "More than one hdf5 file specified.h");

	OmDataLayer dl;
	OmDataReader * hdf5reader = dl.getReader(sourceFilenamesAndPaths[0].filePath(), true );

	hdf5reader->open();

	//get dims of image
	Vector3 < int >dims;
	if(hdf5reader->dataset_exists(dataset)){
		dims = hdf5reader->dataset_image_get_dims( dataset );
	} else {
		dims = hdf5reader->dataset_image_get_dims( OmDataPaths::getDefaultDatasetName() );
	}

	debug("hfd5image", "dims are %i,%i,%i\n", DEBUGV3(dims));

	hdf5reader->close();

	return dims;
}

/////////////////////////////////
///////          vtkImageData Utility Functions
void OmImageDataIo::getVtkExtentFromAxisAlignedBoundingBox(const AxisAlignedBoundingBox < int >&aabb, int extent[])
{
	extent[0] = aabb.getMin().x;
	extent[1] = aabb.getMax().x;
	extent[2] = aabb.getMin().y;
	extent[3] = aabb.getMax().y;
	extent[4] = aabb.getMin().z;
	extent[5] = aabb.getMax().z;
}

void OmImageDataIo::setAxisAlignedBoundingBoxFromVtkExtent(const int extent[], AxisAlignedBoundingBox < int >&aabb)
{
	aabb.setMin(Vector3 < int >(extent[0], extent[2], extent[4]));
	aabb.setMax(Vector3 < int >(extent[1], extent[3], extent[5]));
	aabb.setEmpty(false);
}

void OmImageDataIo::clearImageData(vtkImageData * data)
{
	int dims[3];
	data->GetDimensions(dims);

	int bytes_per_sample = data->GetScalarSize();
	int samples_per_voxel = data->GetNumberOfScalarComponents();

	void *scalar_pointer = data->GetScalarPointer();

	memset(scalar_pointer, 0, bytes_per_sample * samples_per_voxel * dims[0] * dims[1] * dims[2]);
}

OmDataWrapperPtr OmImageDataIo::allocImageData(Vector3 < int >dims, OmDataWrapperPtr old)
{
	//alloc data
	vtkImageData *data = vtkImageData::New();
	debug ("meshercrash", "allocImageData: %p, %i\n", data, data->GetReferenceCount());
	data->SetDimensions(dims.x, dims.y, dims.z);
	data->SetScalarType(bytesToVtkScalarType(old->getSizeof()));
	data->SetNumberOfScalarComponents(1);
	data->AllocateScalars();
	data->Update();

	data->ReleaseDataFlagOn();

	return old->newWrapper(data);
}

OmDataWrapperPtr OmImageDataIo::createBlankImageData(Vector3 < int >dims, OmDataWrapperPtr old, char value)
{
	//alloc data
	OmDataWrapperPtr data = allocImageData(dims, old);

	//clear data
	void *scalar_pointer = data->getVTKPtr()->GetScalarPointer();
	memset(scalar_pointer, value, old->getSizeof() * dims.x * dims.y * dims.z);

	return data;
}

/*
 *	Returns pointer to array of copied data from specified source and bbox.
 */
void * OmImageDataIo::copyImageData(OmDataWrapperPtr srcInData, const DataBbox & srcCopyBbox)
{
	vtkImageData * srcData = srcInData->getVTKPtr();

	//get vtk formatted copy extent
	int src_copy_extent[6];
	getVtkExtentFromAxisAlignedBoundingBox(srcCopyBbox, src_copy_extent);

	//get vtk formmatted data extent
	int src_data_extent[6];
	srcData->GetExtent(src_data_extent);

	//get properties from source
	int scalar_size = srcData->GetScalarSize();
	int num_scalar_components = srcData->GetNumberOfScalarComponents();
	int bytes_per_pixel = scalar_size * num_scalar_components;

	//assert that src can contain copy extent
	//debug("FIXME", << srcCopyBbox << endl;
	//DataBbox temp;
	//setAxisAlignedBoundingBoxFromVtkExtent(src_data_extent, temp);
	//debug("FIXME", << temp << endl;
	assert((src_copy_extent[1] <= src_data_extent[1]) &&
	       (src_copy_extent[3] <= src_data_extent[3]) && (src_copy_extent[5] <= src_data_extent[5]));

	//get continuous increment values
	vtkIdType srcContIncX, srcContIncY, srcContIncZ;
	srcData->GetContinuousIncrements((int *)src_copy_extent, srcContIncX, srcContIncY, srcContIncZ);

	//get scalar pointers
	char *src_data_ptr = static_cast < char *>(srcData->GetScalarPointerForExtent(src_copy_extent));

	//alloc output memory
	const Vector3 < int >copy_bbox_dim = srcCopyBbox.getUnitDimensions();
	void *p_out_data = malloc(copy_bbox_dim.x * copy_bbox_dim.y * copy_bbox_dim.z * bytes_per_pixel);
	memset(p_out_data, 0, copy_bbox_dim.x * copy_bbox_dim.y * copy_bbox_dim.z * bytes_per_pixel);
	char *p_out_data_itr = (char *)p_out_data;
	assert(p_out_data);

	//loop over all slices, rows, and pixels
	for (int z = 0; z < copy_bbox_dim.z; z++) {
		for (int y = 0; y < copy_bbox_dim.y; y++) {
			for (int x = 0; x < copy_bbox_dim.x; x++) {

				memcpy(p_out_data_itr, src_data_ptr, bytes_per_pixel);

				p_out_data_itr += bytes_per_pixel;
				src_data_ptr += bytes_per_pixel;
			}

			//adv continuous inc Y
			src_data_ptr += srcContIncY * bytes_per_pixel;
		}

		//adv continuous inc Z
		src_data_ptr += srcContIncZ * bytes_per_pixel;
	}

	//return pointer to data
	return p_out_data;
}

void OmImageDataIo::copyImageData(OmDataWrapperPtr dstInData, const DataBbox & dstCopyBbox,
				  OmDataWrapperPtr srcInData, const DataBbox & srcCopyBbox)
{
	vtkImageData * dstData = dstInData->getVTKPtr();
	vtkImageData * srcData = srcInData->getVTKPtr();

	//get vtk formatted extent
	int src_copy_extent[6], dst_copy_extent[6];
	getVtkExtentFromAxisAlignedBoundingBox(srcCopyBbox, src_copy_extent);
	getVtkExtentFromAxisAlignedBoundingBox(dstCopyBbox, dst_copy_extent);

	int src_data_extent[6], dst_data_extent[6];
	srcData->GetExtent(src_data_extent);
	dstData->GetExtent(dst_data_extent);

	//get properties from source
	int scalar_size = srcData->GetScalarSize();
	int num_scalar_components = srcData->GetNumberOfScalarComponents();
	int bytes_per_pixel = scalar_size * num_scalar_components;

	///////check dest matches source properties
	//check src and dest dims are the same
	const Vector3 < int >copy_bbox_dim = srcCopyBbox.getMax() - srcCopyBbox.getMin() + Vector3 < int >::ONE;
	Vector3 < int >dest_copy_bbox_dim = dstCopyBbox.getMax() - dstCopyBbox.getMin() + Vector3 < int >::ONE;
	assert(copy_bbox_dim == dest_copy_bbox_dim);

	//assert that src and dest can contain copy extent
	assert((src_copy_extent[1] <= src_data_extent[1]) &&
	       (src_copy_extent[3] <= src_data_extent[3]) && (src_copy_extent[5] <= src_data_extent[5]));
	assert((dst_copy_extent[1] <= dst_data_extent[1]) &&
	       (dst_copy_extent[3] <= dst_data_extent[3]) && (dst_copy_extent[5] <= dst_data_extent[5]));

	//check src and dest have proper scalar type
	assert(dstData->GetScalarSize() == scalar_size);

	//check component count
	assert(dstData->GetNumberOfScalarComponents() == num_scalar_components);
	/////////

	//get continuous increment values
	vtkIdType srcContIncX, srcContIncY, srcContIncZ;
	vtkIdType dstContIncX, dstContIncY, dstContIncZ;
	srcData->GetContinuousIncrements((int *)src_copy_extent, srcContIncX, srcContIncY, srcContIncZ);
	dstData->GetContinuousIncrements((int *)dst_copy_extent, dstContIncX, dstContIncY, dstContIncZ);

	//get scalar pointers
	char *src_data_ptr = static_cast < char *>(srcData->GetScalarPointerForExtent(src_copy_extent));
	assert( src_data_ptr );
	char *dst_data_ptr = static_cast < char *>(dstData->GetScalarPointerForExtent(dst_copy_extent));
	assert( dst_data_ptr );

	//loop over all slices, rows, and pixels
	for (int z = 0; z < copy_bbox_dim.z; z++) {
		for (int y = 0; y < copy_bbox_dim.y; y++) {
#if 1
			for (int x = 0; x < copy_bbox_dim.x; x++) {

				memcpy(dst_data_ptr, src_data_ptr, bytes_per_pixel);
				src_data_ptr += bytes_per_pixel;
				dst_data_ptr += bytes_per_pixel;
			}
#else
			//optimize to copy continuous pixel strips
			memcpy(dst_data_ptr, src_data_ptr, copy_bbox_dim.x * bytes_per_pixel);
			src_data_ptr += copy_bbox_dim.x * bytes_per_pixel;
			dst_data_ptr += copy_bbox_dim.x * bytes_per_pixel;
#endif

			//adv continuous inc Y
			src_data_ptr += srcContIncY * bytes_per_pixel;
			dst_data_ptr += dstContIncY * bytes_per_pixel;
		}

		//adv continuous inc Z
		src_data_ptr += srcContIncZ * bytes_per_pixel;
		dst_data_ptr += dstContIncZ * bytes_per_pixel;
	}
}

void OmImageDataIo::copyIntersectedImageDataFromOffset(OmDataWrapperPtr dstInData, OmDataWrapperPtr srcInData,
						       const Vector3 < int >&srcOffset)
{
	vtkImageData * dstData = dstInData->getVTKPtr();
	vtkImageData * srcData = srcInData->getVTKPtr();

	//bbox of source and destination
	DataBbox src_bbox, dst_bbox;
	setAxisAlignedBoundingBoxFromVtkExtent(srcData->GetExtent(), src_bbox);
	setAxisAlignedBoundingBoxFromVtkExtent(dstData->GetExtent(), dst_bbox);

	//src bbox offset to the dst space
	DataBbox offset_src_bbox = src_bbox;
	offset_src_bbox.offset(srcOffset);

	//dst intersection in dst space
	DataBbox dst_intersection = dst_bbox;

	dst_intersection.intersect(offset_src_bbox);

	//return if nothing to copy
	if (dst_intersection.isEmpty())
		return;

	//src intersection in src space
	DataBbox src_intersection = dst_intersection;
	src_intersection.offset(-srcOffset);

	/*
	   //debug("FIXME", << "copyIntersectedImageDataFromOffset:" << endl;
	   //debug("FIXME", << dst_intersection << endl;
	   //debug("FIXME", << src_intersection << endl;
	 */

	//copy intersected data
	copyImageData(dstInData, dst_intersection, srcInData, src_intersection);
}

/*
 *	Combine pairs of vtkImageData along a given axis.
 *
 *	inputImageData - pairs of vktImageData that will be combined
 *	outputImageData - length is half the number of vtkImageData as the input.
 *	num_pairs is the number of pairs in the input
 *	axis - axis to combine input data
 *
 *	Input data is deleted after output data is created.
 */

void OmImageDataIo::appendImageDataPairs(vtkImageData ** inputImageData, vtkImageData ** outputImageData, int num_pairs, int axis)
{

	for (int i = 0; i < num_pairs; i++) {
		//alloc output data
		outputImageData[i] = vtkImageData::New();

		//create appender
		vtkImageAppend *appender = vtkImageAppend::New();
		appender->AddInput(inputImageData[2 * i]);
		appender->AddInput(inputImageData[2 * i + 1]);
		appender->SetAppendAxis(axis);

		appender->SetOutput(outputImageData[i]);
		appender->Update();
		appender->Delete();

		//erase old image data
		inputImageData[2 * i]->Delete();
		inputImageData[2 * i + 1]->Delete();
	}
}


