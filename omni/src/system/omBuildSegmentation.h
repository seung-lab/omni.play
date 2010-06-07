#ifndef OM_BUILD_SEGMENTATION_H
#define OM_BUILD_SEGMENTATION_H

#include "system/omBuildVolumes.h"
class OmSegmentation;

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
	void doLoadDendrogram();
	void convertToEdgeList( quint32 * dend, float * dendValues, 
				const int size );

	void run();
	void reset();
	void do_build_seg_image();
	void do_build_seg_mesh();
};

#endif
