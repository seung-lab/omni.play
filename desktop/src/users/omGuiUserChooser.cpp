#include "users/omGuiUserChooser.h"
#include "users/omGuiUserChooserWidget.h"

OmGuiUserChooser::OmGuiUserChooser(QWidget* const parent,
                                   om::file::path usersPath)
    : QDialog(parent, Qt::Tool) {
  setAttribute(Qt::WA_DeleteOnClose, true);

  QVBoxLayout* box = new QVBoxLayout();
  box->addWidget(new om::gui::userChooserWidget(this, usersPath));
  setLayout(box);
}
