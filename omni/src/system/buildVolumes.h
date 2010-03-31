#ifndef BUILD_VOLUMES_H
#define BUILD_VOLUMES_H

#include "volume/omChannel.h"
#include "volume/omSegmentation.h"

class BuildVolumes
{
 public:
	BuildVolumes( OmSegmentation * );
	BuildVolumes(OmChannel * );
	void setFileNamesAndPaths( QFileInfoList fileNamesAndPaths );
	void addFileNameAndPath( QString fnp );

	void buildAndMeshSegmentation();
	void build_seg_image();
	void build_seg_mesh();
	void build_channel();

 private:
	QFileInfoList mFileNamesAndPaths;
	OmSegmentation * mSeg;
	OmChannel * mChann;

	time_t time_start;
	time_t time_end;
	double time_dif;

	bool checkSettingsAndTime(QString type);
	void stopTiming(QString type);
};

#endif
