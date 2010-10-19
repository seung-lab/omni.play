#ifndef HEADLESS_IMPL_HPP
#define HEADLESS_IMPL_HPP

#include "project/omProject.h"
#include "volume/omSegmentation.h"
#include "system/omBuildSegmentation.h"

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
};

#endif
