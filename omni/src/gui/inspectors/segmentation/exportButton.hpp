#ifndef SEG_INSPECTOR_EXPORT_BUTTON_H
#define SEG_INSPECTOR_EXPORT_BUTTON_H

#include "datalayer/hdf5/omExportVolToHdf5.hpp"
#include "gui/widgets/omButton.hpp"
#include "gui/inspectors/segmentation/segInspector.h"

class ExportButton : public OmButton<SegInspector> {
public:
    ExportButton(SegInspector * d)
        : OmButton<SegInspector>( d,
                                  "Export and reroot segments",
                                  "Export",
                                  false)
    {
    }

private:
    void doAction()
    {
        const QString fileName =
            QFileDialog::getSaveFileName(this, tr("Export As"));

        if (fileName == NULL)
            return;

        const SegmentationDataWrapper& sdw = mParent->GetSDW();

        OmExportVolToHdf5::Export(sdw.GetSegmentationPtr(), fileName, true);
    }
};

#endif
