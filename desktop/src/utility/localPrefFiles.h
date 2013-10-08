#pragma once

#include "utility/localPrefFilesImpl.hpp"

class LocalPrefFiles : private om::singletonBase<LocalPrefFiles> {
 public:
  static bool SettingExists(const QString& setting) {
    return instance().impl_->settingExists(setting);
  }

  // bool
  inline static bool readSettingBool(const QString& setting,
                                     const bool defaultRet) {
    if (!instance().impl_->settingExists(setting)) {
      return defaultRet;
    }

    try {
      return instance().impl_->readSettingBool(setting);
    }
    catch (...) {
      return defaultRet;
    }
  }

  inline static void writeSettingBool(const QString& setting,
                                      const bool value) {
    return instance().impl_->writeSettingBool(setting, value);
  }

  // numbers
  template <typename T>
  inline static T readSettingNumber(const QString& setting,
                                    const T defaultRet) {
    if (!instance().impl_->settingExists(setting)) {
      return defaultRet;
    }

    try {
      return instance().impl_->readSettingNumber<T>(setting);
    }
    catch (...) {
      return defaultRet;
    }
  }

  template <typename T>
  inline static void writeSettingNumber(const QString& setting, const T value) {
    return instance().impl_->writeSettingNumber<T>(setting, value);
  }

  // QString
  inline static QString readSettingQString(const QString& setting,
                                           const QString& defaultRet) {
    if (!instance().impl_->settingExists(setting)) {
      return defaultRet;
    }

    try {
      return instance().impl_->readSettingQString(setting);
    }
    catch (...) {
      return defaultRet;
    }

  }

  inline static void writeSettingQString(const QString& setting,
                                         const QString& value) {
    return instance().impl_->writeSettingQString(setting, value);
  }

  // QStringList
  inline static QStringList readSettingQStringList(
      const QString& setting, const QStringList& defaultRet) {
    if (!instance().impl_->settingExists(setting)) {
      return defaultRet;
    }

    try {
      return instance().impl_->readSettingQStringList(setting);
    }
    catch (...) {
      return defaultRet;
    }
  }

  inline static void writeSettingQStringList(const QString& setting,
                                             const QStringList& values) {
    return instance().impl_->writeSettingQStringList(setting, values);
  }

 private:
  LocalPrefFiles() : impl_(new LocalPrefFilesImpl()) {}

  om::shared_ptr<LocalPrefFilesImpl> impl_;

  friend class zi::singleton<LocalPrefFiles>;
};
