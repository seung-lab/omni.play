#include "omMeshSource.h"
#include "common/omDebug.h"

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
	debug ("mesher1", "OmMeshSource::~OmMeshSource\n");

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
