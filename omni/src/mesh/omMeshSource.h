#ifndef OM_MESH_SOURCE_H
#define OM_MESH_SOURCE_H

#include "volume/omMipChunk.h"
#include "volume/omMipChunkCoord.h"
#include <vtkImageData.h>

class OmMeshSource {
public:
	OmMeshSource();
	~OmMeshSource();
	
	//load mesh data from chunk - expensive
	void Load( QExplicitlySharedDataPointer<OmMipChunk> chunk );
	
	//copy from another mesh source - cheaper
	void Copy( OmMeshSource &source );
	
	vtkImageData *pImageData;
	OmMipChunkCoord MipCoord;
	int mDecimateLevel;
	AxisAlignedBoundingBox<int> SrcBbox;
	AxisAlignedBoundingBox<float> DstBbox;
	
protected:
        QExplicitlySharedDataPointer<OmMipChunk> mChunk;
	OmMeshSource(OmMeshSource &);
	OmMeshSource& operator= (const OmMeshSource &);
};

#endif
