#ifndef EXPORT_MST_HPP
#define EXPORT_MST_HPP

#include "datalayer/omMST.h"
#include "gui/inspectors/segInspector.h"
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

		SegmentationDataWrapper sdw = mParent->getSegmentationDataWrapper();
		boost::shared_ptr<OmSegmentCache> segmentCache = sdw.getSegmentCache();
		boost::shared_ptr<OmMST> mst = sdw.getSegmentation().getMST();

		const int numEdges = mst->mDendCount;
		const quint32 * nodes    = mst->mDend->getPtr<unsigned int>();
		const float * thresholds = mst->mDendValues->getPtr<float>();

		QStringList headerLabels;
		headerLabels << "Edge" << "segID1" << "segID2" << "threshold"
					 << "seg1size" << "seg2size";
		out << headerLabels.join(",");
		out << "\n";

		for( int i = 0; i < numEdges; ++i){
			const OmSegID node1ID  = nodes[i];
			const OmSegID node2ID  = nodes[i + numEdges ];
			const float threshold  = thresholds[i];

			OmSegment* node1 = segmentCache->GetSegment(node1ID);
			OmSegment* node2 = segmentCache->GetSegment(node2ID);

			out << i << ",";
			out << node1ID << ",";
			out << node2ID << ",";
			out << threshold << ",";
			out << node1->getSize() << ",";
			out << node2->getSize();
			out << "\n";
		}

		printf("\tdone!\n");
	}

};

#endif
