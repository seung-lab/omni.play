#ifndef OM_THREAD_CHUNK_LEVEL_H
#define OM_THREAD_CHUNK_LEVEL_H

/**
 *	Mip Chunks that are not statically sized, but rather sized according to the mip level
 *	which they are at. Building a thread chunk actually results in building multiple
 *	thread chunk level until the root mip level is reached.
 */

#include "omMipChunk.h"

class OmThreadChunkLevel : public OmMipChunk {
 public:
	OmThreadChunkLevel(const OmMipChunkCoord &rMipCoord, OmMipVolume *pMipVolume);
	~OmThreadChunkLevel();

 protected:
	void InitChunk(const OmMipChunkCoord &rMipCoord);
	
 private:
	//image data of chunk
	vtkImageData *mpImageData;	

	friend QDataStream &operator<<(QDataStream & out, const OmMipChunk & chunk );
	friend QDataStream &operator>>(QDataStream & in, OmMipChunk & chunk );
};

#endif
