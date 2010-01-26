#ifndef OM_MIP_CHUNK_MESHER_H
#define OM_MIP_CHUNK_MESHER_H



#include "segment/omSegmentTypes.h"
#include "volume/omMipChunkCoord.h"
#include "system/omSystemTypes.h"

#include "common/omThreads.h"
#include <boost/shared_ptr.hpp>
#include <vmmlib/vmmlib.h>
using namespace vmml;
using boost::shared_ptr;

class OmMipMesh;
class OmMeshSource;
class OmMipMeshManager;

class OmMipChunk;

class vtkPolyData;
class vtkImageData;

/**	
 *	Extracts relavent data from a MipChunk to apply the Marching Cubes algorithm
 *	to build a mesh which is stored as a MipMesh.
 *
 *	Brett Warne - bwarne@mit.edu - 3/7/09
 */

class OmMipChunkMesher {

public:
	static OmMipChunkMesher* Instance();
	static void Delete();
	
	
	static void BuildChunkMeshes(OmMipMeshManager *pMipMeshManager, shared_ptr<OmMipChunk> chunk, const SegmentDataSet &);
	void BuildChunkMeshesThreaded(OmMipMeshManager *pMipMeshManager, shared_ptr<OmMipChunk> chunk,  const SegmentDataSet &);
	SEGMENT_DATA_TYPE GetNextSegmentValueToMesh();
	void BuildMeshesLoop();
	
protected:
	// singleton constructor, copy constructor, assignment operator protected
	OmMipChunkMesher();
	~OmMipChunkMesher();
	OmMipChunkMesher(const OmMipChunkMesher&);
	OmMipChunkMesher& operator= (const OmMipChunkMesher&);
	
	
private:
	//singleton
	static OmMipChunkMesher* mspInstance;
	
	
	//mesh building
	pthread_mutex_t mMeshThreadMutex;
	pthread_cond_t mMeshThreadCv;
	int mMeshThreadCount;
	
	//current data
	OmMipMeshManager *mpMipMeshManager;
	
	OmMeshSource *mpCurrentMeshSource;
	OmMipChunkCoord mCurrentMipCoord;
	SegmentDataSet mCurrentSegmentDataSet;
};



#endif
