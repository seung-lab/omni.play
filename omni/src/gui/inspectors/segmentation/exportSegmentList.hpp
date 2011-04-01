#ifndef EXPORT_SEGMENT_LIST_HPP
#define EXPORT_SEGMENT_LIST_HPP

#include "gui/inspectors/segmentation/segInspector.h"
#include "gui/widgets/omButton.hpp"
#include "segment/omSegments.h"
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
		const SegmentationDataWrapper& sdw = mParent->GetSDW();

		OmSegments* segments = sdw.Segments();

		const QString outFile = OmProject::OmniFile() + ".segments.txt";

		QFile data(outFile);
		if(data.open(QFile::WriteOnly | QFile::Truncate)) {
			printf("writing segment file %s\n", qPrintable(outFile));
		} else{
			throw OmIoException("could not open file", outFile);
		}

		QTextStream out(&data);
		out << "segID,isValid\n";

		for(OmSegID i = 1; i <= segments->getMaxValue(); ++i){
			OmSegment* seg = segments->GetSegment(i);
			if(!seg){
				continue;
			}

			out << i << "," << seg->IsValidListType() << "\n";
		}

		printf("\tdone!\n");
	}
};

#endif
