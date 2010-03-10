#ifndef OM_DATA_VOLUME_H
#define OM_DATA_VOLUME_H

/*
 *	DataVolume provides an interface to accessing values of a data stored in a
 *	volume structure (x,y,z coordinates).
 *
 *	Brett Warne - bwarne@mit.edu - 7/19/09
 */


#include "common/omStd.h"
#include "common/omSerialization.h"
#include "omVolumeTypes.h"

#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
using namespace vmml;

#include <list>
using std::list;


class OmDataVolume {

public:
	OmDataVolume();
	virtual ~OmDataVolume();
	
	//properties	
	virtual const DataBbox& GetExtent() = 0;
	const Vector3<int> GetDimensions();
	const int GetBytesPerSample();
	
	//state
	bool IsOpen() const;
	bool IsDirty() const;

	//data io
	virtual void Open();
	virtual void Flush();
	virtual void Close();
	
	//virtual vtkImageData* Read(const DataBbox &dataExtentBbox, int bytesPerSample);
	//virtual void Write(const DataBbox &dataExtentBbox, int bytesPerSample);
	
	//voxel accessors
	bool ContainsVoxel(const DataCoord &vox);
	virtual uint32_t GetVoxelValue(const DataCoord &vox) = 0;
	virtual void SetVoxelValue(const DataCoord &vox, uint32_t value) = 0;
	

	void SetBytesPerSample(int);
protected:
	void SetOpen(bool);
	
	int mBytesPerSample;		//VTK_UNSIGNED_CHAR (1 byte) or VTK_UNSIGNED_INT (4 bytes)
	
	
private:
	bool mIsOpen;
	
	friend ostream& operator<<(ostream &out, const OmDataVolume &v);
	
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};





/////////////////////////////////
///////		 Serialization


BOOST_CLASS_VERSION(OmDataVolume, 0)

template<class Archive>
void 
OmDataVolume::serialize(Archive & ar, const unsigned int file_version) {

	ar & mBytesPerSample;
}




#endif
