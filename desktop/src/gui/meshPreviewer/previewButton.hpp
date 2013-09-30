#pragma once

#include "gui/widgets/omButton.hpp"
#include "gui/meshPreviewer/meshPreviewer.hpp"

class PreviewButton : public OmButton<MeshPreviewerImpl> {
 public:
  PreviewButton(MeshPreviewerImpl* d)
      : OmButton<MeshPreviewerImpl>(d, "Preview Mesh",
                                    "Compute meshes and display", false) {}

 private:

  void doAction() { mParent->mesh(); }
};
