#pragma once

#include <QTextStream>

class OmFileLogger {
 private:
  QString outFile_;

  std::unique_ptr<QFile> file_;
  std::unique_ptr<QTextStream> out_;

  zi::spinlock lock_;

 public:
  OmFileLogger() {}

  ~OmFileLogger() {
    out_.reset();
    file_.reset();
  }

  void Start(const QString& name) {
    outFile_ = name;

    file_.reset(new QFile(outFile_));

    if (file_->open(QFile::WriteOnly | QFile::Truncate)) {
      printf("writing log file %s\n", qPrintable(outFile_));

    } else {
      throw om::IoException("could not open file", outFile_);
    }

    out_.reset(new QTextStream(file_.get()));
  }

  void Log(const QString& str) {
    zi::guard g(lock_);
    (*out_) << str << "\n";
  }

  void Log(const std::ostringstream& stm) {
    const std::string str = stm.str();
    Log(QString::fromStdString(str));
  }
};
