#ifndef HEADLESS_IMPL_HPP
#define HEADLESS_IMPL_HPP

#include "project/omProject.h"
#include "volume/omSegmentation.h"
#include "system/omBuildSegmentation.h"
#include "utility/dataWrappers.h"
#include "datalayer/omMST.h"

class HeadlessImpl {
public:
	static void buildHDF5(const QString file)
	{
		QString projectFileName = QFileInfo(file+".omni").fileName();

		OmProject::New(projectFileName);

		OmSegmentation& added_segmentation = OmProject::AddSegmentation();
		QString hdf5fnp = file;

		OmBuildSegmentation bs(&added_segmentation);
		bs.addFileNameAndPath(hdf5fnp);
		bs.buildAndMeshSegmentation();
		bs.wait();
	}

	static void importHDF5seg(const std::string& file) {
		importHDF5seg(QString::fromStdString(file));
	}

	static void importHDF5seg(const QString& file)
	{
		QString projectFileName = QFileInfo(file+".omni").fileName();

		OmProject::New(projectFileName);

		OmSegmentation& added_segmentation = OmProject::AddSegmentation();

		OmBuildSegmentation bs(&added_segmentation);
		bs.addFileNameAndPath(file);
		bs.build_seg_image();
		bs.wait();
	}

	static void loadHDF5seg(const QString file, OmID& segmentationID_)
	{
		OmSegmentation& added_segmentation = OmProject::AddSegmentation();
		segmentationID_ = added_segmentation.GetID();

		OmBuildSegmentation bs(&added_segmentation);
		bs.addFileNameAndPath(file);
		bs.build_seg_image();
		bs.wait();
	}

	static void ClearMST(const OmID segmentationID)
	{
		SegmentationDataWrapper sdw(segmentationID);

		boost::shared_ptr<OmMST> mst = sdw.getMST();
		OmMSTEdge* edges = mst->Edges();

		for(uint32_t i = 0; i < mst->NumEdges(); ++i){
			edges[i].userSplit = 0;
			edges[i].userJoin = 0;
			edges[i].wasJoined = 0;
		}

		mst->Flush();

		//clear user edges
	}
};

#endif
