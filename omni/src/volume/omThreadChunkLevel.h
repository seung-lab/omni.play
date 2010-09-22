#ifndef OM_THREAD_CHUNK_LEVEL_H
#define OM_THREAD_CHUNK_LEVEL_H

/**
 *	Mip Chunks that are not statically sized, but rather sized according to the mip level
 *	which they are at. Building a thread chunk actually results in building multiple
 *	thread chunk level until the root mip level is reached.
 */

#include "volume/omMipChunk.h"

class OmThreadChunkLevel : public OmMipChunk {
public:
	OmThreadChunkLevel(const OmMipChunkCoord &, OmMipVolume *);
	~OmThreadChunkLevel();

protected:
	void InitChunk(const OmMipChunkCoord &);

private:
	friend QDataStream &operator<<(QDataStream & out, const OmMipChunk & chunk );
	friend QDataStream &operator>>(QDataStream & in, OmMipChunk & chunk );
};

#endif
