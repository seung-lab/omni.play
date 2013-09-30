#pragma once

#include <QMessageBox>

class OmAskYesNoQuestion : public QMessageBox {
 public:
  OmAskYesNoQuestion() {
    setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    setDefaultButton(QMessageBox::No);
  }

  OmAskYesNoQuestion(const QString& text) {
    setText(text);
    setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    setDefaultButton(QMessageBox::No);
  }

  OmAskYesNoQuestion(const QString& text, const QString& infoText) {
    setText(text);
    setInformativeText(infoText);
    setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    setDefaultButton(QMessageBox::No);
  }

  bool Ask() { return QMessageBox::Yes == exec(); }
};
