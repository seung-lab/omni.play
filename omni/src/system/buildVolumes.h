#ifndef BUILD_VOLUMES_H
#define BUILD_VOLUMES_H

#include <QString>
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"

class BuildVolumes
{
 public:
	BuildVolumes();
	void setFileNamesAndPaths( QFileInfoList fileNamesAndPaths );
	void addFileNameAndPath( QString fnp );

	void buildAndMeshSegmentation( OmSegmentation * current_seg );
	void build_seg_image(OmSegmentation * current_seg);
	void build_seg_mesh(OmSegmentation * current_seg);
	void build_channel(OmChannel * current_channel);

 private:
	QFileInfoList mFileNamesAndPaths;
};


#endif
