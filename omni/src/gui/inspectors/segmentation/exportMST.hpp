#ifndef EXPORT_MST_HPP
#define EXPORT_MST_HPP

#include "datalayer/omMST.h"
#include "gui/inspectors/segmentation/segInspector.h"
#include "gui/widgets/omButton.h"
#include "segment/omSegmentCache.h"
#include "system/omProjectData.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

#include <QTextStream>

class ExportMST : public OmButton<SegInspector> {
public:
	ExportMST(SegInspector * d)
		: OmButton<SegInspector>( d,
								  "Export MST as text file",
								  "export MST",
								  false)
	{}

private:
	void doAction()
	{
		const QString outFile =
			OmProjectData::getAbsoluteFileNameAndPath() + ".mst.txt";
		QFile data(outFile);
		if(data.open(QFile::WriteOnly | QFile::Truncate)) {
			printf("writing segment file %s\n", qPrintable(outFile));
		} else{
			throw OmIoException("could not open file \"" + outFile.toStdString()
								+"\"");
		}

		QTextStream out(&data);

		boost::shared_ptr<SegmentationDataWrapper> sdw =
			mParent->getSegmentationDataWrapper();
		OmSegmentCache* segmentCache = sdw->getSegmentCache();
		boost::shared_ptr<OmMST> mst = sdw->getSegmentation().getMST();
		OmMSTEdge* edges = mst->Edges();

		QStringList headerLabels;
		headerLabels << "Edge" << "segID1" << "segID2" << "threshold"
					 << "userJoin" << "userSplit" << "seg1size" << "seg2size";
		out << headerLabels.join(",");
		out << "\n";

		for(uint32_t i = 0; i < mst->NumEdges(); ++i){
			OmSegment* node1 = segmentCache->GetSegment(edges[i].node1ID);
			OmSegment* node2 = segmentCache->GetSegment(edges[i].node2ID);

			out << i << ",";
			out << edges[i].node1ID << ",";
			out << edges[i].node2ID << ",";
			out << edges[i].threshold << ",";
			out << edges[i].userJoin << ",";
			out << edges[i].userSplit << ",";
			out << node1->getSize() << ",";
			out << node2->getSize();
			out << "\n";
		}

		printf("\tdone!\n");
	}

};

#endif
