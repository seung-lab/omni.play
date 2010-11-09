#ifndef HEADLESS_IMPL_HPP
#define HEADLESS_IMPL_HPP

#include "actions/omActions.hpp"
#include "project/omProject.h"
#include "segment/io/omMST.h"
#include "system/omBuildSegmentation.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "segment/io/omUserEdges.hpp"

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

		sdw.GetSegmentation().getMSTUserEdges()->Clear();
		sdw.GetSegmentation().getMSTUserEdges()->Save();

		OmSegmentCache* segCache = sdw.GetSegmentCache();
		for(OmSegID i = 1; i <= segCache->getMaxValue(); ++i){
			OmSegment* seg = segCache->GetSegment(i);
			if(!seg){
				continue;
			}
			seg->SetListType(om::WORKING);
		}

		OmActions::Save();
	}

	static void RebuildCenterOfSegmentData(const OmID segmentationID)
	{
		SegmentationDataWrapper sdw(segmentationID);

		OmSegmentCache* segCache = sdw.GetSegmentCache();
		for(OmSegID i = 1; i <= segCache->getMaxValue(); ++i){
			OmSegment* seg = segCache->GetSegment(i);
			if(!seg){
				continue;
			}
			seg->clearBounds();
		}

		sdw.GetSegmentation().UpdateVoxelBoundingData();

		OmActions::Save();
	}


	template <typename T>
	static void ChangeVolResolution(T& vol,
									const float xRes,
									const float yRes,
									const float zRes)
	{
		const Vector3f dims(xRes, yRes, zRes);

		vol.SetDataResolution(dims);

		std::cout << "\tvolume data resolution set to "
				  << vol.GetDataResolution()
				  << "\n";
	}
};

#endif
