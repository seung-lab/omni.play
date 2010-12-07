#ifndef SEG_INSPECTOR_EXPORT_BUTTON_RAW_HPP
#define SEG_INSPECTOR_EXPORT_BUTTON_RAW_HPP

#include "gui/widgets/omButton.h"
#include "gui/inspectors/segmentation/segInspector.h"
#include "utility/dataWrappers.h"

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
		sdw.GetSegmentation().ExportInternalData(fileName, false);
	}
};

#endif
