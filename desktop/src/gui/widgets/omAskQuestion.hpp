#pragma once

#include <QMessageBox>

class OmAskQuestion : public QMessageBox {
 public:
  OmAskQuestion() {
    setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    setDefaultButton(QMessageBox::Cancel);
  }

  OmAskQuestion(const QString& text) {
    setText(text);
    setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    setDefaultButton(QMessageBox::Cancel);
  }

  OmAskQuestion(const QString& text, const QString& infoText) {
    setText(text);
    setInformativeText(infoText);
    setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    setDefaultButton(QMessageBox::Cancel);
  }

  bool Ask() { return QMessageBox::Yes == exec(); }
};
