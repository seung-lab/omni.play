#pragma once

#include "utility/omTimer.hpp"
#include "common/logging.h"
#include "project/omProject.h"
#include "actions/omActions.h"
#include "utility/omStringHelpers.h"

#include <QTextStream>
#include <QImage>
#include <QFileInfoList>
#include <QString>
#include <QThread>

class OmBuildVolumes {
 public:
  OmBuildVolumes() {}

  virtual ~OmBuildVolumes() {}

  void setFileNamesAndPaths(const QFileInfoList& fileNamesAndPaths) {
    mFileNamesAndPaths = fileNamesAndPaths.toVector().toStdVector();
  }

  void addFileNameAndPath(const QString& fnp) {
    mFileNamesAndPaths.push_back(QFileInfo(fnp));
    ;
  }

 protected:
  std::vector<QFileInfo> mFileNamesAndPaths;

  bool checkSettings() {
    if (!are_file_names_valid()) {
      throw om::ArgException("Error: file list contains invalid files");
    }

    return true;
  }

  void startTiming(const QString& type, OmTimer& timer) {
    log_infos << "starting " << qPrintable(type) << " build...";
    timer.start();
  }

  void stopTimingAndSave(const QString& type, OmTimer& timer) {
    OmActions::Save();
    const double time = timer.s_elapsed();
    log_info("done: %s build performed in (%.6f secs)", qPrintable(type), time);
    log_infos << "************";
  }

 private:
  bool are_file_names_valid() {
    if (mFileNamesAndPaths.empty()) {
      log_infos << "\tError: can't build: no files selected";
      return false;
    }

    FOR_EACH(iter, mFileNamesAndPaths) {
      if (!iter->exists()) {
        log_infos << "file does not exist: " << qPrintable(iter->filePath());
        return false;
      }

      const QString ext = iter->suffix().toLower();
      if ("tif" == ext || "tiff" == ext || "jpg" == ext || "png" == ext ||
          "h5" == ext || "hdf5" == ext) {

      } else {
        log_infos << "invalid file: " << qPrintable(iter->filePath());
        return false;
      }
    }

    return true;
  }
};
