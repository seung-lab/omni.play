#ifndef EXPORT_SEGMENT_LIST_HPP
#define EXPORT_SEGMENT_LIST_HPP

#include "gui/inspectors/segmentation/segInspector.h"
#include "gui/widgets/omButton.h"
#include "segment/omSegmentCache.h"
#include "system/omProjectData.h"
#include "utility/dataWrappers.h"

#include <QTextStream>

class ExportSegmentList : public OmButton<SegInspector> {
 public:
	ExportSegmentList(SegInspector * d)
		: OmButton<SegInspector>( d,
								  "Export Segment Info (Valid)",
								  "export segment info",
								  false)
	{}

 private:
	void doAction()
	{
		boost::shared_ptr<SegmentationDataWrapper> sdw
			= mParent->getSegmentationDataWrapper();

		OmSegmentCache* segmentCache = sdw->getSegmentCache();

		const QString outFile =
			OmProjectData::getAbsoluteFileNameAndPath() + ".segments.txt";
		QFile data(outFile);
		if(data.open(QFile::WriteOnly | QFile::Truncate)) {
			printf("writing segment file %s\n", qPrintable(outFile));
		} else{
			throw OmIoException("could not open file \"" + outFile.toStdString()
								+"\"");
		}

		QTextStream out(&data);
		out << "segID,isValid\n";

		for(OmSegID i = 1; i <= segmentCache->getMaxValue(); ++i){
			OmSegment* seg = segmentCache->GetSegment(i);
			if(!seg){
				continue;
			}

			out << i << "," << seg->GetImmutable() << "\n";
		}

		printf("\tdone!\n");
	}
};

#endif
