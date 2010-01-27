#ifndef OM_IMAGE_IO_H
#define OM_IMAGE_IO_H

/*
 *
 *
 */

#include "volume/omVolumeTypes.h"
#include "common/omStd.h"

#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
using namespace vmml;

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
namespace bfs = boost::filesystem;

#include <list>
using std::list;

enum ImageType { TIFF_TYPE, JPEG_TYPE, PNG_TYPE, VTK_TYPE, HDF5_TYPE, NONE_TYPE };

class vtkImageReader2;
class vtkImageWriter;
class vtkImageData;

//type
ImageType om_imagedata_parse_image_type(const string & fname);

//vtk io
vtkImageReader2 *om_imagedata_get_reader(ImageType);
vtkImageReader2 *om_imagedata_get_reader(const string & fname);
vtkImageWriter *om_imagedata_get_writer(ImageType);
vtkImageWriter *om_imagedata_get_writer(const string & fname);

//reading
vtkImageData *om_imagedata_read(string dpath, list < string > &fnames, const DataBbox srcExtentBbox,
				const DataBbox dataExtentBbox, int bytesPerSample);
vtkImageData *om_imagedata_read_vtk(string dpath, list < string > &fnames, const DataBbox srcExtentBbox,
				    const DataBbox dataExtentBbox, int bytesPerSample);
vtkImageData *om_imagedata_read_hdf5(string dpath, list < string > &fnames, const DataBbox srcExtentBbox,
				     const DataBbox dataExtentBbox, int bytesPerSample);

//writing
void om_imagedata_write(vtkImageData * data, string dpath, string fpattern, const DataBbox dataExtentBbox,
			int bytesPerSample);
void om_imagedata_write_vtk(vtkImageData * data, string dpath, string fpattern, const DataBbox dataExtentBbox,
			    int bytesPerSample);
void om_imagedata_write_hdf5(vtkImageData * data, string dpath, string fpattern, const DataBbox dataExtentBbox,
			     int bytesPerSample);
void om_imagedata_write_hdf5(vtkImageData * data, string dpath, string fpattern, const DataBbox dstExtentBbox,
			     const DataBbox dataExtentBbox, int bytesPerSample);

//determine dimensions
Vector3 < int >om_imagedata_get_dims(string dpath, const list < string > &fname);
Vector3 < int >om_imagedata_get_dims_vtk(string dpath, const list < string > &fname);
Vector3 < int >om_imagedata_get_dims_hdf5(string dpath, const list < string > &fname);

//symlink data
void om_imagedata_regex_match_dir_contents(string dpath, string regex, list < string > &matches);
void om_imagedata_regex_match_dir_contents_sorted(string dpath, string regexStr, list < string > &matches);
void om_imagedata_create_symlink_dir(string symlink_dpath, string src_dpath, list < string > &src_fnames);
void om_imagedata_remove_symlink_dir(string symlink_dpath);

#pragma mark
#pragma mark vtkImageData Utility Functions
/////////////////////////////////
///////          vtkImageData Utility Functions

void getVtkExtentFromAxisAlignedBoundingBox(const AxisAlignedBoundingBox < int >&aabb, int extent[]);
void setAxisAlignedBoundingBoxFromVtkExtent(const int extent[], AxisAlignedBoundingBox < int >&aabb);

void printImageData(vtkImageData * data);
void *copyImageData(vtkImageData * srcData, const DataBbox & srcCopyBbox);
void copyImageData(vtkImageData * dstData, const DataBbox & dstCopyBbox,
		   vtkImageData * srcData, const DataBbox & srcCopyBbox);
void copyIntersectedImageDataFromOffset(vtkImageData * dstData,
					vtkImageData * srcData, const Vector3 < int >&srcOffset);

void clearImageData(vtkImageData * data);

vtkImageData *allocImageData(Vector3 < int >dims, int bytesPerSample);
vtkImageData *createBlankImageData(Vector3 < int >dims, int bytesPerSample, char value = 0);

void appendImageDataPairs(vtkImageData ** inputImageData, vtkImageData ** outputImageData, int num_pairs, int axis);

////////////////////////////////////////
///////         Utility Functions Prototypes

void makeFilePath(string & fpath, const string & dpath, const string & fpattern, int index);
int countMatchesInDirectory(string & dpath, string & fpattern);

#endif
