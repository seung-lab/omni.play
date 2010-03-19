#include "omImageDataIo.h"
#include "common/omException.h"
#include "utility/omDataLayer.h"
#include "utility/omDataReader.h"
#include "utility/omDataWriter.h"
#include "utility/omHdf5Helpers.h"
#include "common/omVtk.h"

#include <QFile>
#include <QFileInfo>

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

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
namespace bfs = boost::filesystem;

#include <boost/regex.hpp>
//namespace brx=boost::regex;

#include <strnatcmp.h>

#include <hdf5.h>
#include "common/omDebug.h"

#define DEBUG 0

/////////////////////////////////
///////          ImageType Methods

/*
 *	Return image type based on filename extension
 */
ImageType om_imagedata_parse_image_type( QString fileNameAndPath )
{
	QString extension = QFileInfo(fileNameAndPath).suffix();

	//extract file extension
	string ext = "." + extension.toStdString();

	//switch for extention type
	if (".tif" == ext || ".tiff" == ext) {
		return TIFF_TYPE;

	} else if (".jpg" == ext) {
		return JPEG_TYPE;

	} else if (".png" == ext) {
		return PNG_TYPE;

	} else if (".vtk" == ext) {
		return VTK_TYPE;

	} else if (".h5" == ext || ".hdf5" == ext) {
		return HDF5_TYPE;

	} else {
		return NONE_TYPE;
	}
}

/////////////////////////////////
///////          VTK IO Methods

/*
 *	Return pointer to new image reader given an image type.
 */
vtkImageReader2 *om_imagedata_get_reader(ImageType type)
{

	//switch for extention type
	vtkImageReader2 *reader;
	switch (type) {

	case TIFF_TYPE:
		reader = vtkTIFFReader::New();
		break;
	case JPEG_TYPE:
		reader = vtkJPEGReader::New();
		break;
	case PNG_TYPE:
		reader = vtkPNGReader::New();
		break;
	case VTK_TYPE:
		reader = vtkImageReader2::New();
		break;
	case HDF5_TYPE:
	case NONE_TYPE:
		throw OmAccessException("File type not recognized.");

	}

	//read from lowerleft (since it writes from lower left)
	reader->SetFileLowerLeft(1);
	//debug("FIXME", << reader->GetFileLowerLeft() << endl;
	return reader;
}

vtkImageReader2 *om_imagedata_get_reader( QString fname)
{
	return om_imagedata_get_reader(om_imagedata_parse_image_type( fname ) );
}

/*
 *	Return pointer to new image writer given an image type.
 */
vtkImageWriter *om_imagedata_get_writer(ImageType type)
{
	//switch for extention type
	switch (type) {
	case TIFF_TYPE:
		return vtkTIFFWriter::New();
	case JPEG_TYPE:
		return vtkJPEGWriter::New();
	case PNG_TYPE:
		return vtkPNGWriter::New();
	case VTK_TYPE:
		return vtkImageWriter::New();
	case HDF5_TYPE:
	case NONE_TYPE:
	default:
		throw OmAccessException("File type not recognized.");
	}
}

vtkImageWriter *om_imagedata_get_writer(string & fname)
{
	return om_imagedata_get_writer(om_imagedata_parse_image_type( QString::fromStdString(fname)));
}

/////////////////////////////////
///////          Reading Functions

vtkImageData *om_imagedata_read( QFileInfoList sourceFilenamesAndPaths, 
				 const DataBbox srcExtentBbox,
				 const DataBbox dataExtentBbox, 
				 int bytesPerSample)
{

	assert(sourceFilenamesAndPaths.size() && "No files to read.");

	switch (om_imagedata_parse_image_type( sourceFilenamesAndPaths[0].filePath() )){
	case TIFF_TYPE:
	case JPEG_TYPE:
	case PNG_TYPE:
	case VTK_TYPE:
		return om_imagedata_read_vtk( sourceFilenamesAndPaths, srcExtentBbox, dataExtentBbox, bytesPerSample);

	case HDF5_TYPE:
		return om_imagedata_read_hdf5( sourceFilenamesAndPaths, srcExtentBbox, dataExtentBbox, bytesPerSample);

	default:
		assert(false && "Unknown file format");
	}
}

vtkImageData *om_imagedata_read_vtk( QFileInfoList sourceFilenamesAndPaths, 
				     const DataBbox srcExtentBbox,
				     const DataBbox dataExtentBbox, 
				     int bytesPerSample)
{
	//alloc dynamic image data
	vtkImageData *data = vtkImageData::New();

	//get image reader for name specified file type
	vtkImageReader2 *p_reader = om_imagedata_get_reader( sourceFilenamesAndPaths[0].filePath() );

	//generate vtkStringArray
	vtkStringArray *vtk_fpaths_stings = vtkStringArray::New();
	foreach( QFileInfo fi, sourceFilenamesAndPaths ){
		string fnp = fi.filePath().toStdString();
		vtk_fpaths_stings->InsertNextValue( fnp );
	}

	//set filenames
	p_reader->SetFileNames(vtk_fpaths_stings);

	//source extent to read from
	int vtk_src_extent[6];
	getVtkExtentFromAxisAlignedBoundingBox(srcExtentBbox, vtk_src_extent);
	p_reader->SetDataExtent(vtk_src_extent);

	//set scalar type
	p_reader->SetDataScalarType(bytesToVtkScalarType(bytesPerSample));

	//cast to propert type
	//http://www.vtk.org/doc/release/5.0/html/a02398.html#a11
	vtkImageCast *p_caster = vtkImageCast::New();
	p_caster->SetInput(p_reader->GetOutput());
	p_caster->SetOutputScalarType(bytesToVtkScalarType(bytesPerSample));

	//local extent to extract from
	int vtk_local_extent[6];
	getVtkExtentFromAxisAlignedBoundingBox(dataExtentBbox, vtk_local_extent);

	//extract volume of interest in given extent
	vtkExtractVOI *voi_extractor = vtkExtractVOI::New();
	voi_extractor->SetInput(p_caster->GetOutput());
	voi_extractor->SetVOI(vtk_local_extent);

	//pad extent cut by voi extractor
	vtkImageConstantPad *padder = vtkImageConstantPad::New();
	padder->SetInput(voi_extractor->GetOutput());
	padder->SetOutputWholeExtent(vtk_local_extent);

	//normalize extent
	vtkImageTranslateExtent *extent_translator = vtkImageTranslateExtent::New();
	extent_translator->SetInput(padder->GetOutput());
	Vector3 < int >translation = -dataExtentBbox.getMin();
	extent_translator->SetTranslation(translation.array);
	extent_translator->SetOutput(data);
	extent_translator->Update();


	//delete all but image
	p_reader->Delete();
	vtk_fpaths_stings->Delete();
	p_caster->Delete();
	voi_extractor->Delete();
	padder->Delete();
	extent_translator->Delete();

	return data;
}

vtkImageData *om_imagedata_read_hdf5( QFileInfoList sourceFilenamesAndPaths, 
				      const DataBbox srcExtentBbox,
				      const DataBbox dataExtentBbox, 
				      int bytesPerSample)
{
	//FIXME: don't assert, or check before calling me!
	assert((sourceFilenamesAndPaths.size() == 1) && "More than one hdf5 file specified.h");

	OmDataLayer dl;
	OmDataReader * hdf5reader = dl.getReader( sourceFilenamesAndPaths[0].filePath(), true );

	vtkImageData *data = hdf5reader->dataset_image_read_trim( OmHdf5Helpers::getDefaultDatasetName(),
								   dataExtentBbox, 
								   bytesPerSample);
	return data;
}

/*
 *	Destination extent is data extent when not specified.
 */

/////////////////////////////////
///////          Dimensions Functions
Vector3 < int > om_imagedata_get_dims( QFileInfoList sourceFilenamesAndPaths )
{
	assert(sourceFilenamesAndPaths.size() && "No files specified");

	//use first name in list to determine filetype
	switch (om_imagedata_parse_image_type( sourceFilenamesAndPaths[0].filePath() )){
	case TIFF_TYPE:
	case JPEG_TYPE:
	case PNG_TYPE:
	case VTK_TYPE:
		return om_imagedata_get_dims_vtk( sourceFilenamesAndPaths );

	case HDF5_TYPE:
		return om_imagedata_get_dims_hdf5( sourceFilenamesAndPaths );

	default:
		assert(false && "Unknown file format");
	}
}

Vector3 < int > om_imagedata_get_dims_vtk( QFileInfoList sourceFilenamesAndPaths )
{
	////use vtk to determine properties
	vtkImageData *data = vtkImageData::New();

	//set reader props
	vtkImageReader2 *reader = om_imagedata_get_reader( sourceFilenamesAndPaths[0].filePath() );
	string firstFileNameAndPath = sourceFilenamesAndPaths[0].filePath().toStdString();
	reader->SetFileName( firstFileNameAndPath.c_str() );
	reader->SetOutput(data);
	reader->Update();

	//get dim information
	Vector3 < int >src_dims;

	//set slice dimensions
	data->GetDimensions(src_dims.array);

	//set num slices
	src_dims.z = sourceFilenamesAndPaths.size();

	//delete image and reader
	data->Delete();
	reader->Delete();

	return src_dims;
}

Vector3 < int > om_imagedata_get_dims_hdf5( QFileInfoList sourceFilenamesAndPaths )
{
	assert((sourceFilenamesAndPaths.size() == 1) && "More than one hdf5 file specified.h");

	OmDataLayer dl;
	OmDataReader * hdf5reader = dl.getReader(sourceFilenamesAndPaths[0].filePath(), true );

	//get dims of image
	Vector3 < int >dims = hdf5reader->dataset_image_get_dims( OmHdf5Helpers::getDefaultDatasetName() );

	debug("hfd5image", "dims are %i,%i,%i\n", DEBUGV3(dims));
	return dims;
}

/////////////////////////////////
///////          vtkImageData Utility Functions
void getVtkExtentFromAxisAlignedBoundingBox(const AxisAlignedBoundingBox < int >&aabb, int extent[])
{
	extent[0] = aabb.getMin().x;
	extent[1] = aabb.getMax().x;
	extent[2] = aabb.getMin().y;
	extent[3] = aabb.getMax().y;
	extent[4] = aabb.getMin().z;
	extent[5] = aabb.getMax().z;
}

void setAxisAlignedBoundingBoxFromVtkExtent(const int extent[], AxisAlignedBoundingBox < int >&aabb)
{
	aabb.setMin(Vector3 < int >(extent[0], extent[2], extent[4]));
	aabb.setMax(Vector3 < int >(extent[1], extent[3], extent[5]));
	aabb.setEmpty(false);
}

void printImageData(vtkImageData * data)
{
	int extent[6];
	data->GetExtent(extent);

	int num_components = data->GetNumberOfScalarComponents();

	for (int z = extent[4]; z <= extent[5]; ++z) {
		for (int y = extent[2]; y <= extent[3]; ++y) {
			for (int x = extent[0]; x <= extent[1]; ++x) {

				if (num_components == 1) {
					//debug("FIXME", << data->GetScalarComponentAsFloat(x, y, z, 0) << " ";
				} else {
					//debug("FIXME", << "( ";
					//for (int i = 0; i < num_components; ++i)
						//debug("FIXME", << data->GetScalarComponentAsFloat(x, y, z, i) << " ";
					//debug("FIXME", << ")";
				}

			}
			//debug("FIXME", << endl;
		}
		//debug("FIXME", << endl;
	}
}

void clearImageData(vtkImageData * data)
{
	int dims[3];
	data->GetDimensions(dims);

	int bytes_per_sample = data->GetScalarSize();
	int samples_per_voxel = data->GetNumberOfScalarComponents();

	void *scalar_pointer = data->GetScalarPointer();

	memset(scalar_pointer, 0, bytes_per_sample * samples_per_voxel * dims[0] * dims[1] * dims[2]);
}

vtkImageData *allocImageData(Vector3 < int >dims, int bytesPerSample)
{
	//alloc data
	vtkImageData *data = vtkImageData::New();
	debug ("meshercrash", "allocImageData: %p, %i\n", data, data->GetReferenceCount());
	data->SetDimensions(dims.x, dims.y, dims.z);
	data->SetScalarType(bytesToVtkScalarType(bytesPerSample));
	data->SetNumberOfScalarComponents(1);
	data->AllocateScalars();
	data->Update();

	data->ReleaseDataFlagOn();

	return data;
}

vtkImageData *createBlankImageData(Vector3 < int >dims, int bytesPerSample, char value)
{
	//alloc data
	vtkImageData *data = allocImageData(dims, bytesPerSample);

	//clear data
	void *scalar_pointer = data->GetScalarPointer();
	memset(scalar_pointer, value, bytesPerSample * dims.x * dims.y * dims.z);
	return data;
}

/*
 *	Returns pointer to array of copied data from specified source and bbox.
 */
void *copyImageData(vtkImageData * srcData, const DataBbox & srcCopyBbox)
{

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

void
copyImageData(vtkImageData * dstData, const DataBbox & dstCopyBbox,
	      vtkImageData * srcData, const DataBbox & srcCopyBbox)
{

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

void copyIntersectedImageDataFromOffset(vtkImageData * dstData, vtkImageData * srcData, const Vector3 < int >&srcOffset)
{

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
	copyImageData(dstData, dst_intersection, srcData, src_intersection);
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

void appendImageDataPairs(vtkImageData ** inputImageData, vtkImageData ** outputImageData, int num_pairs, int axis)
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
