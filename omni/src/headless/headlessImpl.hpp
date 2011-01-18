#ifndef HEADLESS_IMPL_HPP
#define HEADLESS_IMPL_HPP

#include "segment/omSegmentLists.hpp"
#include "segment/omSegmentUtils.hpp"
#include "mesh/omMeshParams.hpp"
#include "actions/omActions.hpp"
#include "project/omProject.h"
#include "segment/io/omMST.h"
#include "volume/build/omBuildSegmentation.hpp"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "segment/io/omUserEdges.hpp"
#include "utility/omColorUtils.hpp"
#include "volume/build/omWatershedImporter.hpp"

class HeadlessImpl {
public:
	static void OpenProject(const QString& fNameIn)
	{
		if("" == fNameIn){
			printf("no project file name\n");
			return;
		}

		QString fName = fNameIn;
		if(!fNameIn.endsWith(".omni")){
			fName = fNameIn + ".omni";
		}

		try {
			printf("Please wait: Opening project \"%s\"...\n", qPrintable(fName));
			OmProject::Load(fName);
			//Set current working directory to file path
			QDir::setCurrent(QFileInfo(fName).absolutePath());
			printf("Opened project \"%s\"\n", qPrintable(fName));
		} catch(...) {
			printf("Error while loading project \"%s\"\n", qPrintable(fName));
		}
	}

	static void buildHDF5(const QString file)
	{
		QString projectFileName = QFileInfo(file+".omni").fileName();

		OmProject::New(projectFileName);

		OmBuildSegmentation bs;
		bs.addFileNameAndPath(file);
		bs.BuildAndMeshSegmentation(om::BLOCKING);
	}

	static void importHDF5seg(const std::string& file) {
		importHDF5seg(QString::fromStdString(file));
	}

	static void importHDF5seg(const QString& file)
	{
		QString projectFileName = QFileInfo(file+".omni").fileName();

		OmProject::New(projectFileName);

		OmBuildSegmentation bs;
		bs.addFileNameAndPath(file);
		bs.BuildImage(om::BLOCKING);
	}

	static void loadHDF5seg(const QString file, OmID& segmentationID_)
	{
		OmBuildSegmentation bs;
		bs.addFileNameAndPath(file);
		bs.BuildImage(om::BLOCKING);

		segmentationID_ = bs.GetDataWrapper().getID();
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

	static void RecolorAllSegments(const OmID segmentationID)
	{
		SegmentationDataWrapper sdw(segmentationID);

		OmSegmentCache* segCache = sdw.GetSegmentCache();
		for(OmSegID i = 1; i <= segCache->getMaxValue(); ++i){
			OmSegment* seg = segCache->GetSegment(i);
			if(!seg){
				continue;
			}
			seg->RandomizeColor();
		}

		OmActions::Save();
 	}

	static void RebuildCenterOfSegmentData(const OmID segmentationID)
	{
		SegmentationDataWrapper sdw(segmentationID);
		OmSegmentUtils::RebuildCenterOfSegmentData(sdw);
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

	static void SetMeshDownScallingFactor(const double factor)
	{
		OmMeshParams::SetDownScallingFactor(factor);
		std::cout << "mesh downscalling factor set to "
				  << OmMeshParams::GetDownScallingFactor()
				  << "\n";
	}

	static void ReValidateEveryObject(const OmID segmentationID)
	{
     	OmSegmentListContainer<OmSegmentListBySize>& validList =
			SegmentationDataWrapper(segmentationID).GetSegmentation().GetSegmentLists()->Valid();

		OmSegIDsSet allSegIDs = validList.List().AllSegIDs();

		FOR_EACH(iter, allSegIDs){
			SegmentDataWrapper sdw(segmentationID, *iter);
			OmActions::ValidateSegment(sdw, om::SET_VALID);
		}

		OmActions::Save();
	}

	static void DumpSegmentColorHistograms(const OmID segmentationID){
		dumpRootSegmentColorHistograms(segmentationID, false);
	}

	static void DumpRootSegmentColorHistograms(const OmID segmentationID){
		dumpRootSegmentColorHistograms(segmentationID, true);
	}

private:
	static void dumpRootSegmentColorHistograms(const OmID segmentationID,
											   const bool findRoot)
	{
		SegmentationDataWrapper sdw(segmentationID);

		std::map<OmColor, int> segColorHist;

        static const int min_variance = 120;

		for(int r = 0; r < 128; ++r){
			for(int g = 0; g < 128; ++g){
				for(int b = 0; b < 128; ++b){
					const int avg  = ( r + g + b ) / 3;
					const int avg2 = ( r*r + g*g + b*b ) / 3;
					const int v = avg2 - avg*avg;
					if(v >= min_variance){
						const OmColor color = {r, g, b};
						segColorHist[color] = 0;
					}
				}
			}
		}

		std::cout << "found "
				  << OmStringHelpers::CommaDeliminateNum(segColorHist.size())
				  << " colors\n";

		OmSegmentCache* segCache = sdw.GetSegmentCache();
		for(OmSegID i = 1; i <= segCache->getMaxValue(); ++i){
			OmSegment* seg = segCache->GetSegment(i);
			if(!seg){
				continue;
			}
			if(findRoot){
				seg = segCache->findRoot(seg);
			}
			const OmColor color = seg->GetColorInt();

			++(segColorHist[color]);
		}

		QString outFile("/tmp/segmentColors.txt");
		if(findRoot){
			outFile = "/tmp/segmentRootColors.txt";
		}

		QFile data(outFile);
		if(data.open(QFile::WriteOnly | QFile::Truncate)) {
			printf("writing segment file %s\n", qPrintable(outFile));
		} else{
			throw OmIoException("could not open file", outFile);
		}

		QTextStream out(&data);

		out << "red\tgreen\tblue\tnum\n";

		FOR_EACH(iter, segColorHist){
			out << iter->first << "\t"
				<< iter->second << "\n";
		}
	}

public:
	static void ImportWatershed(const QString& fnp)
	{
		SegmentationDataWrapper sdw;
		OmSegmentation& segmentation = sdw.Create();

		OmWatershedImporter importer(segmentation, fnp);
		importer.Import();
	}
};

#endif
