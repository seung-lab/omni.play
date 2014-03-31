#pragma once
#include "precomp.h"

#include "datalayer/file.h"

class OmGuiUserChooser : public QDialog {
 public:
  OmGuiUserChooser(QWidget *const parent, om::file::path usersPath);
};
