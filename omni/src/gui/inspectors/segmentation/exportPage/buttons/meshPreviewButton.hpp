#ifndef SEG_INSPECTOR_MESH_PREVIEW_BUTTON_H
#define SEG_INSPECTOR_MESH_PREVIEW_BUTTON_H

#include "gui/widgets/omButton.hpp"
#include "gui/inspectors/segmentation/exportPage/pageExport.h"
#include "gui/meshPreviewer/meshPreviewer.hpp"

namespace om {
namespace segmentationInspector {

class MeshPreviewButton : public OmButton<PageExport> {
public:
    MeshPreviewButton(PageExport * d)
        : OmButton<PageExport>( d,
                                "Mesh Preview",
                                "preview mesh",
                                false)
    {}

private:
    void doAction()
    {
        // const SegmentationDataWrapper& sdw = mParent->GetSDW();

        // new MeshPreviewer(this, sdw, mParent->GetViewGroupState());
    }
};

} // namespace segmentationInspector
} // namespace om

#endif
