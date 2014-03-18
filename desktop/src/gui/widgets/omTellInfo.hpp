#pragma once
#include "precomp.h"

class OmTellInfo : public QMessageBox {
 public:
  OmTellInfo() {}

  OmTellInfo(const std::string& text) {
    setText(QString::fromStdString(text));
    exec();
  }

  OmTellInfo(const QString& text) {
    setText(text);
    exec();
  }

  OmTellInfo(const char* text) {
    setText(QString(text));
    exec();
  }
};
