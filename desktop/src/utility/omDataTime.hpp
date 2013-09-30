#pragma once

#include <QDateTime>

namespace om {
namespace datetime {

std::string cur() {
  return QDateTime::currentDateTime().toString("ddd MMMM d yy hh:mm:ss.zzz")
      .toStdString();
}

}  // namespace datetime
}  // namespace om
