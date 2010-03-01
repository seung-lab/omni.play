#ifndef OM_MESH_SOURCE_H
#define OM_MESH_SOURCE_H

#include "volume/omMipChunk.h"
#include "volume/omMipChunkCoord.h"
#include <vtkImageData.h>
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

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

#endif
