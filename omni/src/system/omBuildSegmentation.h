#ifndef OM_BUILD_SEGMENTATION_H
#define OM_BUILD_SEGMENTATION_H

#include "system/omBuildVolumes.h"
#include "volume/omSegmentation.h"

class OmBuildSegmentation : public OmBuildVolumes, public QThread
{
 public:
	OmBuildSegmentation( OmSegmentation * );

	void buildAndMeshSegmentation();
	void build_seg_image();
	void build_seg_mesh();
	void loadDendrogram();

 private:
	OmSegmentation * mSeg;

	bool doBuildImage;
	bool doBuildMesh;

	void run();
	void reset();
	void do_build_seg_image();
	void do_build_seg_mesh();
};

#endif
