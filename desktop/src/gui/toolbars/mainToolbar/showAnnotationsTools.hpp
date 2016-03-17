#pragma once
#include "precomp.h"

#include "gui/widgets/omButton.hpp"
#include "gui/annotationToolbox/annotationToolbox.h"

class ShowAnnotationsTools : public OmButton {
 private:
  om::gui::AnnotationToolbox* annotationToolbox_;

 public:
  ShowAnnotationsTools(QWidget* parent, OmViewGroupState& vgs)
      : OmButton(parent, "", "Show annotation tools", true),
        annotationToolbox_(new om::gui::AnnotationToolbox(parent, vgs)) {
    SetIcon(":/toolbars/mainToolbar/icons/folder_documents.png");
  }

 private:
  void onLeftClick() override {
    if (!isChecked()) {
      annotationToolbox_->show();

    } else {
      annotationToolbox_->hide();
    }
  }
};
