#pragma once

#include "common/common.h"
#include "actions/io/omActionTypes.h"

#include <QFileInfo>
#include <QTextStream>
#include <QStringList>

class OmActionDumper {
 private:
  std::unique_ptr<QTextStream> out_;

 public:
  OmActionDumper() {}

  void Dump(const QString& fnp);

 private:
  void readAndDumpFile(const QFileInfo& fileInfo);
  void dispatchAction(const QString& actionName, QDataStream& in,
                      const QString& fileName);

  static QString getDataAndTime(const QString& fnp) {
    const QStringList parts = fnp.split("--");
    if (parts.size() < 2) {
      return "";
    }
    return parts[0] + "--" + parts[1];
  }

  template <typename ACTION, typename IMPL>
  void doDumpFile(QDataStream& in, const QString& fnp) {
    IMPL impl;
    in >> impl;

    QTextStream& out = *out_;

    out << impl.classNameForLogFile();
    out << "\t";
    out << getDataAndTime(fnp);
    out << "\t";
    out << impl;
    out << "\n";
  }
};
