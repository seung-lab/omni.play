
#include "omMipChunkMesher.h"
#include "omMipMesh.h"
#include "omMipMeshManager.h"
#include "omMesher.h"
#include "segment/omSegment.h"

#include "volume/omMipChunk.h"

#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"

#include "common/omGl.h"


#include <vtkImageData.h>
#include <vtkFloatArray.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkTransform.h>

#include <vtkMarchingCubes.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkThreshold.h>
#include "system/omDebug.h"
#import <vtkCleanPolyData.h>
#import <vtkSmoothPolyDataFilter.h>
#import <vtkPolyDataNormals.h>
#import <vtkStripper.h>
#import <vtkPolyData.h>
#import <vtkTransformPolyDataFilter.h>
#import <vtkQuadricDecimation.h>

#import <vtkDecimatePro.h>

#include <sys/wait.h>

#define DEBUG 0



//utility
uint32_t numStripsInStripCellArray(vtkCellArray *pStripCellArray);
void* init_meshing_thread(void* arg);


//init instance pointer
OmMipChunkMesher* OmMipChunkMesher::mspInstance = 0;




#pragma mark -
#pragma mark OmMipChunkMesher
/////////////////////////////////
///////
///////		 OmMipChunkMesher
///////


OmMipChunkMesher::OmMipChunkMesher() {
	//debug("genone","OmMipChunkMesher::OmMipChunkMesher()");
	
	//init thread count
	mMeshThreadCount = 0;
	
	//init mesh source
	mpCurrentMeshSource = NULL;
	
	//mesh building
	pthread_mutex_init(&mMeshThreadMutex, NULL);
	pthread_cond_init(&mMeshThreadCv, NULL);
}


OmMipChunkMesher::~OmMipChunkMesher() {
	//debug("genone","OmMipChunkMesher::~OmMipChunkMesher()");
}



OmMipChunkMesher* 
OmMipChunkMesher::Instance() {
	if(NULL == mspInstance) {
		mspInstance = new OmMipChunkMesher;
	}
	return mspInstance;
}


void 
OmMipChunkMesher::Delete() {
	if(mspInstance) delete mspInstance;
	mspInstance = NULL;
}







#pragma mark 
#pragma mark Threaded Chunk Meshing
/////////////////////////////////
///////		 Threaded Chunk Meshing



void 
OmMipChunkMesher::BuildChunkMeshes(OmMipMeshManager *pMipMeshManager, shared_ptr <OmMipChunk> chunk, const SegmentDataSet &rMeshVals) {
     debug("mesher","OmMipChunkMesher::BuildChunkMeshes: (x,y,z):%i,%i,%i\n",
           chunk->GetCoordinate().get<1>(),
           chunk->GetCoordinate().get<2>(),
           chunk->GetCoordinate().get<3>());
	

	Instance()->BuildChunkMeshesThreaded(pMipMeshManager, chunk, rMeshVals);

}

void 
OmMipChunkMesher::BuildChunkMeshesThreaded(OmMipMeshManager *pMipMeshManager, shared_ptr<OmMipChunk> chunk, const SegmentDataSet &rMeshVals) {
	
	//set manager
	mpMipMeshManager = pMipMeshManager;
	
	const int num_threads = OmPreferences::GetInteger(OM_PREF_MESH_NUM_MESHING_THREADS_INT);
	debug("mesher","------> Number of threads: %d\n", num_threads );
	
	//SET CURRENT DATA
	//make meshing data available to threads
	mCurrentSegmentDataSet = rMeshVals;			//chunk.GetDirectDataValues();
	//but don't mesh null data (used to mark no more data to mesh)
	mCurrentSegmentDataSet.erase(NULL_SEGMENT_DATA);
	//load current mesh source
	mpCurrentMeshSource = new OmMeshSource();
	mpCurrentMeshSource->Load(chunk);
	//current chunk
	mCurrentMipCoord = chunk->GetCoordinate();
	chunk->Open ();
	

	debug("mesher","Should have chunk loaded now: ");
	
//#define OM_SINGLE_THREADED_MESHER
#ifdef OM_SINGLE_THREADED_MESHER
	init_meshing_thread ((void *)this);
#else
	
	//lock thread mutex to prevent created threads from starting
	pthread_mutex_lock(&mMeshThreadMutex);

	//create all meshing threads
	pthread_t meshing_threads[num_threads];
	for(int i=0; i<num_threads; ++i) {
		pthread_create(&meshing_threads[i], NULL, init_meshing_thread, (void *)this);
          	debug("thread","OmMipChunkMesher::BuildChunkMeshesThreaded->Thread Created"); 
	}
	
	//free mutex and with for signal
	pthread_cond_wait(&mMeshThreadCv, &mMeshThreadMutex);

	//all threads must be dead, so free mutex and return
	pthread_mutex_unlock(&mMeshThreadMutex);

#endif

	delete mpCurrentMeshSource;
	mpCurrentMeshSource = NULL;
	
	return;
}



/*
 *	Returns NULL_SEGMENT_DATA if no more values in set
 */
SEGMENT_DATA_TYPE 
OmMipChunkMesher::GetNextSegmentValueToMesh() {
	SEGMENT_DATA_TYPE segment_value;
	
	pthread_mutex_lock(&mMeshThreadMutex);
	
	if(mCurrentSegmentDataSet.size() == 0) {
		segment_value = NULL_SEGMENT_DATA;
	} else {
		//pop value
		segment_value = *mCurrentSegmentDataSet.begin();
		mCurrentSegmentDataSet.erase(segment_value);
		assert(NULL_SEGMENT_DATA != segment_value);
	}
	
	pthread_mutex_unlock(&mMeshThreadMutex);
	
	return segment_value;
}


void 
OmMipChunkMesher::BuildMeshesLoop() {
	
	//init thread index
	int thread_index = -1;
	bool dosignal = false;
	
	pthread_mutex_lock(&mMeshThreadMutex);
	
	//init mesher
	OmMesher mesher(*mpCurrentMeshSource);
	
	//get thread index, and inc count
	thread_index = mMeshThreadCount++;
	debug("mesher","OmMipChunkMesher::BuildMeshesLoop(): %p %i \n", pthread_self(), thread_index);
	
	pthread_mutex_unlock(&mMeshThreadMutex);
	
	
	
	//loop for each available segment value
	while(true) {
		
		//get next segment value
		SEGMENT_DATA_TYPE segment_value = GetNextSegmentValueToMesh();
		
		//if null, then no more values in set, so break loop
		if(NULL_SEGMENT_DATA == segment_value) break;
		
		//get mesh coordiante
		OmMipMeshCoord mesh_coord = OmMipMeshCoord( mCurrentMipCoord, segment_value );
		//cout << "OmMipChunkMesher::BuildMeshesLoop(): thread: " << thread_index << " mesh: " << mesh_coord << endl;
		
		//get alloc'd mesh
		OmMipMesh *p_mesh = mpMipMeshManager->AllocMesh(mesh_coord);
		
		//build mesh data using thread's mesh source and seg value
		mesher.ExtractMesh(p_mesh, segment_value);
		
		//save mesh
		p_mesh->Save();
		
		//delete mesh
		delete p_mesh;
	}
	
	//no more seg values avail, dec thread count
	pthread_mutex_lock(&mMeshThreadMutex);
	mMeshThreadCount--;
	cout << "mMeshThreadCount: " << mMeshThreadCount << endl;
	if(0 == mMeshThreadCount)
		dosignal = true;
	pthread_mutex_unlock(&mMeshThreadMutex);
	
	//if last thread, then signal
	if(dosignal) {
		cout << "mMeshThreadCount is 0 so ... signaling" << endl;
	 	pthread_cond_signal(&mMeshThreadCv);
	}
}






void*
init_meshing_thread(void* arg) {
	
	//cast into manager
	OmMipChunkMesher *p_manager = (OmMipChunkMesher*) arg;
	p_manager->BuildMeshesLoop();
	
#ifdef OM_SINGLE_THREADED_MESHER
	pthread_exit(NULL);
#endif
}


