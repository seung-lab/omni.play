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
};

#endif
