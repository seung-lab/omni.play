#pragma once

#include "common/common.h"
#include <QString>

class OmDataPath {
 private:
  std::string path_;

 public:
  OmDataPath() {}

  OmDataPath(const char* str) : path_(std::string(str)) {}

  OmDataPath(const std::string& str) : path_(str) {}

  OmDataPath(const QString& str) : path_(str.toStdString()) {}

  void setPath(const char* str) { path_ = std::string(str); }

  void setPath(const std::string& str) { path_ = str; }

  void setPath(const QString& str) { path_ = str.toStdString(); }

  const std::string& getString() const { return path_; }

  const QString getQString() const { return QString::fromStdString(path_); }

  friend std::ostream& operator<<(std::ostream& out, const OmDataPath& in) {
    out << "\"" << in.path_ << "\"";
    return out;
  }
};
