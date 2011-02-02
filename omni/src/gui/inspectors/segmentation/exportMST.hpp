#ifndef EXPORT_MST_HPP
#define EXPORT_MST_HPP

#include "segment/io/omMST.h"
#include "gui/inspectors/segmentation/segInspector.h"
#include "gui/widgets/omButton.hpp"
#include "segment/omSegmentCache.h"
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
		const QString outFile = OmProject::OmniFile() + ".mst.txt";

		QFile data(outFile);
		if(data.open(QFile::WriteOnly | QFile::Truncate)) {
			printf("writing segment file %s\n", qPrintable(outFile));
		} else{
			throw OmIoException("could not open file", outFile);
		}

		QTextStream out(&data);

		const SegmentationDataWrapper& sdw = mParent->GetSegmentationDataWrapper();
		OmSegmentCache* segmentCache = sdw.SegmentCache();
		OmMST* mst = sdw.MST();
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
			out << node1->size() << ",";
			out << node2->size();
			out << "\n";
		}

		printf("\tdone!\n");
	}

};

#endif
