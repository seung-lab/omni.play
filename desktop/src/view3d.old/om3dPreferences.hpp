#pragma once
#include "precomp.h"

#include "common/common.h"
#include "zi/omUtility.h"
#include "utility/localPrefFiles.h"

class Om3dPreferences : private om::singletonBase<Om3dPreferences> {
 private:
  int mCrosshairValue;
  bool m2DViewFrameIn3D;
  bool mDrawCrosshairsIn3D;
  bool mDoDiscoBall;
  bool m2DViewPaneIn3D;

 public:
  static uint32_t getCrosshairValue() { return instance().mCrosshairValue; }
  static void setCrosshairValue(const uint32_t value) {
    instance().mCrosshairValue = value;
    LocalPrefFiles::writeSettingNumber<uint32_t>("crosshairValue", value);
  }

  static bool get2DViewFrameIn3D() { return instance().m2DViewFrameIn3D; }
  static void set2DViewFrameIn3D(const bool value) {
    instance().m2DViewFrameIn3D = value;
    LocalPrefFiles::writeSettingBool("2DViewFrameIn3D", value);
  }

  static bool get2DViewPaneIn3D() { return instance().m2DViewPaneIn3D; }
  static void set2DViewPaneIn3D(const bool value) {
    instance().m2DViewPaneIn3D = value;
    LocalPrefFiles::writeSettingBool("2DViewPaneIn3D", value);
  }

  static bool getDrawCrosshairsIn3D() { return instance().mDrawCrosshairsIn3D; }
  static void setDrawCrosshairsIn3D(const bool value) {
    instance().mDrawCrosshairsIn3D = value;
    LocalPrefFiles::writeSettingBool("DrawCrosshairsIn3D", value);
  }

  static bool getDoDiscoBall() { return instance().mDoDiscoBall; }
  static void setDoDiscoBall(const bool value) {
    instance().mDoDiscoBall = value;
    LocalPrefFiles::writeSettingBool("DoDiscoBall", value);
  }

 private:
  Om3dPreferences() {
    setDefaultCrosshairValue();
    setDefault2DViewFrameIn3D();
    setDefaultDrawCrosshairsIn3D();
    setDefaultDoDiscoBall();
    setDefault2DViewPaneIn3D();
  }
  ~Om3dPreferences() {}

  void setDefaultCrosshairValue() {
    const uint32_t defaultRet = 1;
    mCrosshairValue = LocalPrefFiles::readSettingNumber<uint32_t>(
        "crosshairValue", defaultRet);
  }

  void setDefault2DViewFrameIn3D() {
    const bool defaultRet = false;
    m2DViewFrameIn3D =
        LocalPrefFiles::readSettingBool("2DViewFrameIn3D", defaultRet);
  }

  void setDefaultDrawCrosshairsIn3D() {
    const bool defaultRet = false;
    mDrawCrosshairsIn3D =
        LocalPrefFiles::readSettingBool("DrawCrosshairsIn3D", defaultRet);
  }

  void setDefaultDoDiscoBall() {
    const bool defaultRet = false;
    mDoDiscoBall = LocalPrefFiles::readSettingBool("DoDiscoBall", defaultRet);
  }

  void setDefault2DViewPaneIn3D() {
    const bool defaultRet = false;
    m2DViewPaneIn3D =
        LocalPrefFiles::readSettingBool("2DViewPaneIn3D", defaultRet);
  }

  friend class zi::singleton<Om3dPreferences>;
};
