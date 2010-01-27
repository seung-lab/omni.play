
#include "omImageDataIo.h"

#include "system/omException.h"
#include "utility/omHdf5.h"
#include "common/omVtk.h"

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
#include "system/omDebug.h"

#define DEBUG 0

static const char *HDF5_DEFAULT_DATASET_NAME = "main";

#pragma mark -
#pragma mark ImageType Methods
/////////////////////////////////
///////          ImageType Methods

/*
 *	Return image type based on filename extension
 */
ImageType om_imagedata_parse_image_type(const string & fname)
{
	//extract file extension
	string ext = bfs::path(fname).extension();

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

#pragma mark -
#pragma mark VTK IO Methods
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
		//vtkTIFFReader *tiff_reader = vtkTIFFReader::New();
		//tiff_reader->SetOrientationType(ORIENTATION_TOPLEFT);
		//reader = (vtkImageReader2*) tiff_reader; 
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

 defaut:
		throw OmAccessException("File type not recognized.");

	}

	//read from lowerleft (since it writes from lower left)
	reader->SetFileLowerLeft(1);
	//cout << reader->GetFileLowerLeft() << endl;
	return reader;
}

vtkImageReader2 *om_imagedata_get_reader(const string & fname)
{
	return om_imagedata_get_reader(om_imagedata_parse_image_type(fname));
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

	}

	throw OmAccessException("File type not recognized.");
}

vtkImageWriter *om_imagedata_get_writer(string & fname)
{
	return om_imagedata_get_writer(om_imagedata_parse_image_type(fname));
}

#pragma mark -
#pragma mark Reading Functions
/////////////////////////////////
///////          Reading Functions

vtkImageData *om_imagedata_read(string dpath, list < string > &fnames, const DataBbox srcExtentBbox,
				const DataBbox dataExtentBbox, int bytesPerSample)
{

	assert(fnames.size() && "No files to read.");

	switch (om_imagedata_parse_image_type(fnames.front())) {
	case TIFF_TYPE:
	case JPEG_TYPE:
	case PNG_TYPE:
	case VTK_TYPE:
		return om_imagedata_read_vtk(dpath, fnames, srcExtentBbox, dataExtentBbox, bytesPerSample);

	case HDF5_TYPE:
		return om_imagedata_read_hdf5(dpath, fnames, srcExtentBbox, dataExtentBbox, bytesPerSample);

	default:
		assert(false && "Unknown file format");
	}
}

vtkImageData *om_imagedata_read_vtk(string dpath, list < string > &fnames, const DataBbox srcExtentBbox,
				    const DataBbox dataExtentBbox, int bytesPerSample)
{
	//debug("genone","om_imagedata_read_vtk");

	//alloc dynamic image data
	vtkImageData *data = vtkImageData::New();

	//get image reader for name specified file type
	vtkImageReader2 *p_reader = om_imagedata_get_reader(fnames.front());

	//generate vtkStringArray
	vtkStringArray *vtk_fpaths_stings = vtkStringArray::New();
	list < string >::iterator itr;
	for (itr = fnames.begin(); itr != fnames.end(); itr++) {
		vtk_fpaths_stings->InsertNextValue(dpath + *itr);
	}

	//set filenames
	p_reader->SetFileNames(vtk_fpaths_stings);

	//set reader props
	//p_reader->SetFilePrefix(dpath.c_str());
	//convert to vtk "%s/FilePattern" format
	//string vtk_file_pattern = "%s" + fpattern;
	//p_reader->SetFilePattern(vtk_file_pattern.c_str());

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

	//      cout << "UPDATE" << endl;
	//      cout << dpath << vtk_file_pattern << endl;
	//cout << "om_imagedata_read" << data->GetScalarSize() << endl;

	//delete all but image
	p_reader->Delete();
	vtk_fpaths_stings->Delete();
	p_caster->Delete();
	voi_extractor->Delete();
	padder->Delete();
	extent_translator->Delete();

	return data;
}

vtkImageData *om_imagedata_read_hdf5(string dpath, list < string > &fnames, const DataBbox srcExtentBbox,
				     const DataBbox dataExtentBbox, int bytesPerSample)
{

	//assert only one hdf5 file specified
	assert((fnames.size() == 1) && "More than one hdf5 file specified.h");

	string fpath = dpath + fnames.front();

	//open file
	hid_t file_id = om_hdf5_file_open(fpath.c_str());
	//write image data
	vtkImageData *data =
	    om_hdf5_dataset_image_read_trim(file_id, HDF5_DEFAULT_DATASET_NAME, dataExtentBbox, bytesPerSample);
	//close file
	om_hdf5_file_close(file_id);

	return data;
}

#pragma mark -
#pragma mark Writing Functions
/////////////////////////////////
///////         Writing Functions

void
om_imagedata_write(vtkImageData * data, string dpath, string fpattern, const DataBbox dataExtentBbox,
		   int bytesPerSample)
{

	switch (om_imagedata_parse_image_type(fpattern)) {
	case TIFF_TYPE:
	case JPEG_TYPE:
	case VTK_TYPE:
		om_imagedata_write_vtk(data, dpath, fpattern, dataExtentBbox, bytesPerSample);
		break;

	case HDF5_TYPE:
		om_imagedata_write_hdf5(data, dpath, fpattern, dataExtentBbox, bytesPerSample);
		break;

	default:
		assert(false && "Unsupported file format");
	}
}

void
om_imagedata_write_vtk(vtkImageData * data, string dpath, string fpattern, const DataBbox dataExtentBbox,
		       int bytesPerSample)
{

	//debug("genone","Write");

	//convert to vtk extent
	int vtk_extent[6];
	getVtkExtentFromAxisAlignedBoundingBox(dataExtentBbox, vtk_extent);

	//extract volume of interest
	vtkExtractVOI *voi_extractor = vtkExtractVOI::New();
	voi_extractor->SetInput(data);
	voi_extractor->SetVOI(vtk_extent);

	//pad extent cut by voi extractor
	vtkImageConstantPad *padder = vtkImageConstantPad::New();
	padder->SetInput(voi_extractor->GetOutput());
	padder->SetOutputWholeExtent(vtk_extent);

	//flip
	vtkImageFlip *flipper = vtkImageFlip::New();
	flipper->SetInput(padder->GetOutput());
	flipper->SetFilteredAxis(1);

	//normalize extent
	vtkImageTranslateExtent *extent_translator = vtkImageTranslateExtent::New();
	extent_translator->SetInput(flipper->GetOutput());
	Vector3 < int >translation = -dataExtentBbox.getMin();
	extent_translator->SetTranslation(translation.array);

	//cast data
	vtkImageCast *p_caster = vtkImageCast::New();
	p_caster->SetInput(extent_translator->GetOutput());
	p_caster->SetOutputScalarType(bytesToVtkScalarType(bytesPerSample));

	//create image writer
	vtkImageWriter *writer = om_imagedata_get_writer(fpattern);
	writer->SetInput(p_caster->GetOutput());
	writer->SetFilePrefix(dpath.c_str());

	//convert to vtk "%s/FilePattern" format
	string vtk_file_pattern = "%s" + fpattern;
	writer->SetFilePattern(vtk_file_pattern.c_str());

	//create output directory
	bfs::create_directories(bfs::path(dpath));

	//writer->Update();
	writer->Write();

	//delete all but image
	writer->Delete();
	p_caster->Delete();
	voi_extractor->Delete();
	padder->Delete();
	flipper->Delete();
	extent_translator->Delete();
}

/*
 *	Destination extent is data extent when not specified.
 */
void
om_imagedata_write_hdf5(vtkImageData * data, string dpath, string fpattern, const DataBbox dataExtentBbox,
			int bytesPerSample)
{
	om_imagedata_write_hdf5(data, dpath, fpattern, dataExtentBbox, dataExtentBbox, bytesPerSample);
}

void
om_imagedata_write_hdf5(vtkImageData * data, string dpath, string fpattern, const DataBbox dstExtentBbox,
			const DataBbox dataExtentBbox, int bytesPerSample)
{

	//create file if needed
	string fpath = dpath + fpattern;
	if (!bfs::exists(bfs::path(fpath))) {

		//create file
		om_hdf5_file_create(fpath.c_str());
		//open
		hid_t file_id = om_hdf5_file_open(fpath.c_str());

		//create image file
		Vector3 < int >dest_dims = dstExtentBbox.getUnitDimensions();
		om_hdf5_dataset_image_create_tree_overwrite(file_id, HDF5_DEFAULT_DATASET_NAME, dest_dims, dest_dims,
							    bytesPerSample, true);

		//close file
		om_hdf5_file_close(file_id);
	}
	//open file
	hid_t file_id = om_hdf5_file_open(fpath.c_str());
	//write image data
	om_hdf5_dataset_image_write_trim(file_id, HDF5_DEFAULT_DATASET_NAME, dataExtentBbox, bytesPerSample, data);
	//close file
	om_hdf5_file_close(file_id);
}

#pragma mark -
#pragma mark Dimensions Functions
/////////////////////////////////
///////          Dimensions Functions

/*
Vector3<int> 
om_imagedata_get_dims(string dpath, string fpattern) {
	
	switch(om_imagedata_parse_image_type(fpattern)) {
		case TIFF_TYPE:
		case JPEG_TYPE:
		case PNG_TYPE:
		case VTK_TYPE:
			return om_imagedata_get_dims_vtk(dpath, fpattern);
			
		case HDF5_TYPE:
			return om_imagedata_get_dims_hdf5(dpath, fpattern);
			
		default:
			assert(false && "Unknown file format");
	}
}

Vector3<int> 
om_imagedata_get_dims_vtk(string dpath, string fpattern) {
	////use vtk to determine properties
	vtkImageData *data = vtkImageData::New();
	
	//convert to vtk "%s/FilePattern" format
	string vtk_file_pattern = "%s" + fpattern;
	
	//set reader props
	vtkImageReader2 *reader = om_imagedata_get_reader(fpattern);
	reader->SetFilePrefix(dpath.c_str());
	reader->SetFilePattern(vtk_file_pattern.c_str());
	reader->SetOutput(data);
	reader->Update();
	
	//get dim information
	Vector3<int> src_dims;
	data->GetDimensions(src_dims.array);
	src_dims.z = countMatchesInDirectory(dpath, fpattern);

	//get sample information
	//mSourceBytesPerSample = mImageData->GetScalarSize();
	
	//cout << mSourceDataExtent << endl;
	//cout << mSamplesPerVoxel << endl;
	//cout << mBytesPerSample << endl;
	
	//delete image and reader
	data->Delete();
	reader->Delete();
	
	return src_dims;
}

Vector3<int> 
om_imagedata_get_dims_hdf5(string dpath, string fpattern) {

	//get fileid
	string fpath = dpath + fpattern;
	hid_t file_id = om_hdf5_file_open(fpath.c_str());
	
	//get dims of image
	Vector3<int> dims = om_hdf5_dataset_image_get_dims(file_id, HDF5_DEFAULT_DATASET_NAME);
	
	//close file
	om_hdf5_file_close(file_id);
	
	return dims;
}
*/

Vector3 < int > om_imagedata_get_dims(string dpath, const list < string > &fnames)
{

	assert(fnames.size() && "No files specified");

	//use first name in list to determine filetype
	switch (om_imagedata_parse_image_type(fnames.front())) {
	case TIFF_TYPE:
	case JPEG_TYPE:
	case PNG_TYPE:
	case VTK_TYPE:
		return om_imagedata_get_dims_vtk(dpath, fnames);

	case HDF5_TYPE:
		return om_imagedata_get_dims_hdf5(dpath, fnames);

	default:
		assert(false && "Unknown file format");
	}
}

Vector3 < int > om_imagedata_get_dims_vtk(string dpath, const list < string > &fnames)
{

	////use vtk to determine properties
	vtkImageData *data = vtkImageData::New();

	//convert to vtk "%s/FilePattern" format
	//string vtk_file_pattern = "%s" + fpattern;    

	//set reader props
	vtkImageReader2 *reader = om_imagedata_get_reader(fnames.front());
	reader->SetFileName((dpath + fnames.front()).c_str());
	reader->SetOutput(data);
	reader->Update();

	//get dim information
	Vector3 < int >src_dims;
	//set slice dimensions
	data->GetDimensions(src_dims.array);
	//set num slices
	src_dims.z = fnames.size();

	//get sample information
	//mSourceBytesPerSample = mImageData->GetScalarSize();

	//cout << mSourceDataExtent << endl;
	//cout << mSamplesPerVoxel << endl;
	//cout << mBytesPerSample << endl;

	//delete image and reader
	data->Delete();
	reader->Delete();

	return src_dims;
}

Vector3 < int > om_imagedata_get_dims_hdf5(string dpath, const list < string > &fnames)
{

	//assert only one hdf5 file specified
	assert((fnames.size() == 1) && "More than one hdf5 file specified.h");

	//get fileid
	string fpath = dpath + fnames.front();
	hid_t file_id = om_hdf5_file_open(fpath.c_str());

	//get dims of image
	Vector3 < int >dims = om_hdf5_dataset_image_get_dims(file_id, HDF5_DEFAULT_DATASET_NAME);

	//close file
	om_hdf5_file_close(file_id);

	return dims;
}

#pragma mark -
#pragma mark vtkImageData Utility Functions
/////////////////////////////////
///////
///////          vtkImageData Utility Functions
///////

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
	float f;

	for (int z = extent[4]; z <= extent[5]; ++z) {
		for (int y = extent[2]; y <= extent[3]; ++y) {
			for (int x = extent[0]; x <= extent[1]; ++x) {

				if (num_components == 1) {
					cout << data->GetScalarComponentAsFloat(x, y, z, 0) << " ";
				} else {
					cout << "( ";
					for (int i = 0; i < num_components; ++i)
						cout << data->GetScalarComponentAsFloat(x, y, z, i) << " ";
					cout << ")";
				}

			}

			//end of x line
			cout << endl;
		}

		//end of y sheet
		cout << endl;
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
	data->SetDimensions(dims.x, dims.y, dims.z);
	data->SetScalarType(bytesToVtkScalarType(bytesPerSample));
	data->SetNumberOfScalarComponents(1);
	data->AllocateScalars();
	data->Update();

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
	//cout << srcCopyBbox << endl;
	//DataBbox temp;
	//setAxisAlignedBoundingBoxFromVtkExtent(src_data_extent, temp);
	//cout << temp << endl;
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
	char *dst_data_ptr = static_cast < char *>(dstData->GetScalarPointerForExtent(dst_copy_extent));

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
	   cout << "copyIntersectedImageDataFromOffset:" << endl;
	   cout << dst_intersection << endl;
	   cout << src_intersection << endl;
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

#pragma mark -
#pragma mark Symlink Data
/////////////////////////////////
///////          Symlink Data

/*
 *	Regex to find all valid filenames in a given directory
 *	All results must have the same extention type.
 */
void om_imagedata_regex_match_dir_contents(string dpath, string regexStr, list < string > &rMatchFnames)
{

	//clear vector
	rMatchFnames.clear();

	//try to setup regular expression
	boost::regex re;
	try {
		re.assign(regexStr);
	} catch(boost::regex_error & e) {
		cout << "om_imagedata_regex_contents: " << regexStr << "is not a valid regular expression" << endl;
		return;
	}

	//bfs path to directory
	bfs::path dpath_bfs(dpath);

	//if does not exist or not a directory
	if (!bfs::exists(dpath_bfs) || !bfs::is_directory(dpath_bfs)) {
		//cout << "om_imagedata_regex_contents: source directory does not exist" << endl;
		return;
	}
	//for all contents
	string ext;
	bfs::directory_iterator end;
	for (bfs::directory_iterator itr(dpath_bfs); itr != end; ++itr) {

		//ignore directories
		if (bfs::is_directory(*itr))
			continue;

		//for each file path, get leaf file name
		string fname = itr->leaf();

		//ignore if not a match
		if (!boost::regex_match(fname, re))
			continue;

		//else valid match so store
		rMatchFnames.push_back(fname);
	}
}

/*
 
 //if extension not yet set
 if(0 == ext.size()) {
 ext = bfs::path(fname).extension();
 } else {
 //ext must match
 if(ext != bfs::path(fname).extension()) {
 //else clear matches and return
 rMatchFnames.clear();
 return;
 }
 }
 */

bool string_natural_comparison(const string & lhs, const string & rhs)
{
	return strnatcmp(lhs.c_str(), rhs.c_str()) < 0;
}

/*
 *	Given a directory path and a regular expression, creates a list of matching filenames
 *	sorted by natural ordering.
 */
void om_imagedata_regex_match_dir_contents_sorted(string dpath, string regexStr, list < string > &rMatchFnames)
{

	//clear vector
	rMatchFnames.clear();

	//try to setup regular expression
	boost::regex re;
	try {
		re.assign(regexStr);
	} catch(boost::regex_error & e) {
		cout << "om_imagedata_regex_contents: " << regexStr << "is not a valid regular expression" << endl;
		return;
	}

	//bfs path to directory
	bfs::path dpath_bfs(dpath);

	//if does not exist or not a directory
	if (!bfs::exists(dpath_bfs) || !bfs::is_directory(dpath_bfs)) {
		cout << "om_imagedata_regex_match_dir_contents_sorted: source directory does not exist" << endl;
		return;
	}
	//create a set that sorts by natrual string comparison
	set < string, bool(*)(const string & lhs, const string & rhs)>match_nat_cmp_set(string_natural_comparison);

	//for all contents
	string ext;
	bfs::directory_iterator end;
	for (bfs::directory_iterator itr(dpath_bfs); itr != end; ++itr) {

		//ignore directories
		if (bfs::is_directory(*itr))
			continue;

		//for each file path, get leaf file name
		string fname = itr->leaf();

		//ignore if not a match
		if (!boost::regex_match(fname, re))
			continue;

		//else valid match so store
		match_nat_cmp_set.insert(fname);
	}

	//convert set to a list
	set < string, int (*) (const string & lhs, const string & rhs)>::iterator match_itr;
	for (match_itr = match_nat_cmp_set.begin(); match_itr != match_nat_cmp_set.end(); match_itr++) {
		rMatchFnames.push_back(*match_itr);
	}
}

/*
 *	Given a source directory and list of source file names, create a directory of symlinks
 *	with sequentially labed files.
 */
void om_imagedata_create_symlink_dir(string symlinkDpath, string srcDpath, list < string > &rSrcFnames)
{

	//ignore if no src_files
	if (rSrcFnames.size() == 0)
		return;

	//remove output directory if it already exists
	om_imagedata_remove_symlink_dir(symlinkDpath);

	//create output directory
	if (!bfs::create_directories(bfs::path(symlinkDpath))) {
		cout << "om_imagedata_create_symlink_dir: could not create temp dirctory" << endl;
		assert(false);
	}
	//for all filenames in the list
	string src_fpath, symlink_fpath;
	list < string >::iterator itr;
	int i = 0;
	for (itr = rSrcFnames.begin(); itr != rSrcFnames.end(); itr++, i++) {

		//form real path
		src_fpath = srcDpath + *itr;

		//check real path exists
		if (!bfs::exists(bfs::path(src_fpath))) {
			cout << "om_imagedata_create_symlink_dir: source not found " << src_fpath << endl;
			assert(false);
		}
		//make symlink path
		makeFilePath(symlink_fpath, symlinkDpath, "temp.%d.tif", i);

		//create symlink from symlink_path to src_path
		create_symlink(bfs::path(src_fpath), bfs::path(symlink_fpath));
		if (!bfs::exists(bfs::path(symlink_fpath))) {
			cout << "om_imagedata_create_symlink_dir: could not create symlink: " << symlink_fpath << endl;
			assert(false);
		}
	}
}

void om_imagedata_remove_symlink_dir(string symlink_dpath)
{

	//if temp directory exists
	if (bfs::exists(bfs::path(symlink_dpath))) {

		//attemp to remove temp directory
		if (!bfs::remove_all(bfs::path(symlink_dpath))) {
			cout << "om_imagedata_remove_symlink_dir: could not remove temp dirctory" << endl;
			assert(false);
		}

	}

}

#pragma mark -
#pragma mark Utility Functions
/////////////////////////////////
///////
///////         Utility Functions
///////

/* 
 * Generates filepath from given directory path, file format, and index value.
 * on error, returns -1
 */
void makeFilePath(string & fpath, const string & dpath, const string & fpattern, int index)
{

	char formatted_fname_buf[MAX_FNAME_SIZE];
	int str_len = 0;

	//try to create formatted filename
	if ((str_len = sprintf(formatted_fname_buf, fpattern.c_str(), index)) < 0)
		throw OmFormatException("Could not create formatted filename");

	//check formatted string fits in buffer
	if (str_len > MAX_FNAME_SIZE)
		throw OmFormatException("Filename too long.");

	//set to directory path
	fpath = dpath;

	//and append filename
	fpath.append(formatted_fname_buf);
}

/*
 * Returns the number of files that match given fpattern in directory dpath.  
 * Enumerates starting from 1 to fill format string with "%d" charactors in it.
 */

int countMatchesInDirectory(string & dpath, string & fpattern)
{

	int file_index = 0;

	while (true) {

		//get next filepath
		string fpath;
		try {
			makeFilePath(fpath, dpath, fpattern, file_index);

		}
		catch(OmFormatException e) {
			fprintf(stderr, "Could not get next file path\n");
			continue;
		}

		//is file path valid
		if (!bfs::exists(bfs::path(fpath)))
			break;

		//else, inc file index
		file_index = file_index + 1;
	}

	return file_index;
}
