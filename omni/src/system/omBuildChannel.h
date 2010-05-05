#ifndef OM_BUILD_CHANNEL_H
#define OM_BUILD_CHANNEL_H

#include "system/omBuildVolumes.h"
#include "volume/omChannel.h"

class OmBuildChannel : public OmBuildVolumes, public QThread
{
 public:
	OmBuildChannel(OmChannel * );
	void build_channel();

 private:
	OmChannel * mChann;
	void run();

	void newBuild();
	int mDepth;
	Vector3 < int > MipLevelDataDimensions(int level);
	bool BuildVolume();
	void BuildChunk(const OmMipChunkCoord & rMipCoord);
	DataBbox MipCoordToDataBbox(const OmMipChunkCoord & rMipCoord, int newLevel);
};

#endif
