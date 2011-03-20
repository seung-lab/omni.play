#ifndef SEG_INSPECTOR_EXPORT_BUTTON_RAW_HPP
#define SEG_INSPECTOR_EXPORT_BUTTON_RAW_HPP

#include "gui/widgets/omButton.hpp"
#include "gui/inspectors/segmentation/segInspector.h"
#include "gui/inspectors/segmentation/segInspector.h"

class ExportButtonRaw : public OmButton<SegInspector> {
public:
	ExportButtonRaw(SegInspector * d)
		: OmButton<SegInspector>( d,
								  "Export Raw",
								  "Export Raw",
								  false)
	{}

private:
	void doAction()
	{
		const QString fileName =
			QFileDialog::getSaveFileName(this, tr("Export As"));

		if (fileName == NULL)
			return;

		const SegmentationDataWrapper& sdw = mParent->GetSegmentationDataWrapper();

		OmExportVolToHdf5::Export(sdw.GetSegmentationPtr(), fileName, false);
	}
};

#endif
