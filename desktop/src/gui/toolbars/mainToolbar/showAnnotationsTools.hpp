#pragma once

#include "gui/widgets/omButton.hpp"
#include "gui/annotationToolbox/annotationToolbox.h"

class ShowAnnotationsTools : public OmButton<QWidget> {
 private:
  om::gui::AnnotationToolbox* annotationToolbox_;

 public:
  ShowAnnotationsTools(QWidget* parent, OmViewGroupState* vgs)
      : OmButton<QWidget>(parent, "", "Show annotation tools", true),
        annotationToolbox_(new om::gui::AnnotationToolbox(parent, vgs)) {
    SetIcon(":/toolbars/mainToolbar/icons/folder_documents.png");
  }

 private:
  void doAction() {
    if (!isChecked()) {
      annotationToolbox_->show();

    } else {
      annotationToolbox_->hide();
    }
  }

};
