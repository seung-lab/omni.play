#pragma once
#include "precomp.h"

#include "common/common.h"
#include "zi/utility.h"
#include "utility/localPrefFiles.h"
#include "utility/systemInformation.h"
#include "system/cache/omCacheManager.h"

class OmLocalPreferences : private om::SingletonBase<OmLocalPreferences> {
 public:
  // font
  static bool FontSizeWasSet() {
    return LocalPrefFiles::SettingExists("fontSize");
  }

  static int32_t FontSize() {
    return LocalPrefFiles::readSettingNumber<int32_t>("fontSize", 12);
  }

  static void FontSize(const int fontSize) {
    LocalPrefFiles::writeSettingNumber<int32_t>("fontSize", fontSize);
  }

  // max number of mesh worker threads
  static int32_t numAllowedWorkerThreads() {
    const int numCoresRaw = om::utility::systemInformation::get_num_cores();
    int numCores = numCoresRaw - 1;
    if (numCoresRaw < 2) {
      numCores = 2;
    }

    return LocalPrefFiles::readSettingNumber<int32_t>("numThreads", numCores);
  }

  static void setNumAllowedWorkerThreads(const int32_t numThreads) {
    LocalPrefFiles::writeSettingNumber<int32_t>("numThreads", numThreads);
  }

  // RAM size
  static uint32_t getMeshCacheSizeMB() {
    auto defaultRet =
        om::utility::systemInformation::get_total_system_memory_megs() / 3;
    return LocalPrefFiles::readSettingNumber<uint32_t>("ram", defaultRet);
  }

  static void setMeshCacheSizeMB(const uint32_t size) {
    LocalPrefFiles::writeSettingNumber<uint32_t>("ram", size);
    OmCacheManager::UpdateCacheSizeFromLocalPrefs();
  }

  // VRAM size
  static uint32_t getTileCacheSizeMB() {
    auto defaultRet =
        om::utility::systemInformation::get_total_system_memory_megs() / 4;
    return LocalPrefFiles::readSettingNumber<uint32_t>("vram", defaultRet);
  }

  static void setTileCacheSizeMB(const uint32_t size) {
    LocalPrefFiles::writeSettingNumber<uint32_t>("vram", size);
    OmCacheManager::UpdateCacheSizeFromLocalPrefs();
  }

  // recently-used files
  static QStringList getRecentlyUsedFilesNames() {
    QStringList empty;
    return LocalPrefFiles::readSettingQStringList("recentlyOpenedFiles", empty);
  }

  static void setRecentlyUsedFilesNames(const QStringList& values) {
    LocalPrefFiles::writeSettingQStringList("recentlyOpenedFiles", values);
  }

  // scratch space
  static QString getScratchPath() {
    QString defaultPath = "/tmp";
    return LocalPrefFiles::readSettingQString("sratchPath", defaultPath);
  }

  static void setScratchPath(const QString& scratchPath) {
    LocalPrefFiles::writeSettingQString("sratchPath", scratchPath);
  }

  // jump to next segment
  static bool GetShouldJumpToNextSegmentAfterValidate() {
    const bool defaultVal = true;
    return LocalPrefFiles::readSettingBool(
        "shouldJumpToNextSegmentAfterValidate", defaultVal);
  }

  static void SetShouldJumpToNextSegmentAfterValidate(const bool val) {
    LocalPrefFiles::writeSettingBool("shouldJumpToNextSegmentAfterValidate",
                                     val);
  }

  // Login endpoint
  static QString getEndpoint() {
    QString defaultEndpoint = "https://eyewire.org";
    return LocalPrefFiles::readSettingQString("endpoint", defaultEndpoint);
  }

  static void setEndpoint(const QString& endpoint) {
    LocalPrefFiles::writeSettingQString("endpoint", endpoint);
  }

  // Login username
  static QString getUsername() {
    QString defaultUsername = "";
    return LocalPrefFiles::readSettingQString("username", defaultUsername);
  }

  static void setUsername(const QString& username) {
    LocalPrefFiles::writeSettingQString("username", username);
  }

  // Dateset (Eyewire vs Omni)
  static int getDataset() {
    // Default to Omni (id=2)
    return LocalPrefFiles::readSettingNumber("dataset", 2);
  }

  static void setDataset(int dataset) {
    LocalPrefFiles::writeSettingNumber("dataset", dataset);
  }

 private:
  OmLocalPreferences() {}
  ~OmLocalPreferences() {}

  friend class zi::singleton<OmLocalPreferences>;
};
