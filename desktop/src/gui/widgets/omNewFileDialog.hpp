#pragma once
#include "precomp.h"

class OmNewFileDialog {
 private:
  QWidget* const parent_;
  const QString caption_;
  const QString fileNameFilter_;

 public:
  OmNewFileDialog(QWidget* const parent)
      : parent_(parent),
        caption_("New Project"),
        fileNameFilter_("Omni (*.omni)") {}

  /**
   * query user for new file name; if user omits ".omni" extension, and adding
   *  the extension would cause a pre-existing project to be overwritten
   *  without the user's consent, loop until the user confirms the file
   *  overwrite (or renames the file)
   **/
  QString GetNewFileName() {
    const QString fnp_raw = newDialog();
    if (nullptr == fnp_raw) {
      return nullptr;
    }

    const QString fnp =
        om::file::Paths(fnp_raw.toStdString()).OmniFile().c_str();
    if (fnp == fnp_raw) {
      return fnp;
    }
    if (!QFile::exists(fnp)) {
      return fnp;
    }

    return loopUntilStringsMatch(fnp);
  }

 private:
  QString loopUntilStringsMatch(const QString& old) {
    const QString fnp = newDialog(old);
    if (nullptr == fnp) {
      return nullptr;
    }
    if (fnp == old) {
      return fnp;
    }
    if (!QFile::exists(fnp)) {
      return fnp;
    }

    return loopUntilStringsMatch(fnp);
  }

  QString newDialog() {
    return QFileDialog::getSaveFileName(parent_, caption_, "", fileNameFilter_);
  }

  QString newDialog(const QString& fnp) {
    return QFileDialog::getSaveFileName(parent_, caption_, fnp,
                                        fileNameFilter_);
  }
};
