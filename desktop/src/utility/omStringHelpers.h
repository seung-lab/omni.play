#pragma once
#include "precomp.h"

#include "common/common.h"
#include "common/string.hpp"
#include "zi/omUtility.h"

class OmStringHelpers {
 public:
  template <typename T>
  inline static QString HumanizeNumQT(const T num, const char sep = ',') {
    return QString::fromStdString(om::string::humanizeNum(num, sep));
  }

  static uint32_t getUInt(const QString& arg) {
    bool ok;
    uint32_t ret = arg.toUInt(&ok, 10);
    if (ok) {
      return ret;
    }
    throw om::IoException("could not parse to uint32_t");
  }

  static int32_t getInt(const QString& arg) {
    bool ok;
    uint32_t ret = arg.toInt(&ok, 10);
    if (ok) {
      return ret;
    }
    throw om::IoException("could not parse to int32_t");
  }

  static bool getBool(const QString& arg) {
    if ("true" == arg) {
      return true;
    }
    if ("false" == arg) {
      return false;
    }

    return getUInt(arg);
  }

  static double getDouble(const QString& arg) {
    bool ok;
    double ret = arg.toDouble(&ok);
    if (ok) {
      return ret;
    }
    throw om::IoException("could not parse to double");
  }

  static float getFloat(const QString& arg) {
    bool ok;
    float ret = arg.toFloat(&ok);
    if (ok) {
      return ret;
    }
    throw om::IoException("could not parse to float");
  }
};
