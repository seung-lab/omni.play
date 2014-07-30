#pragma once
#include "precomp.h"

#include "common/common.h"
#include "zi/utility.h"
#include "utility/localPrefFiles.h"

class Om3dPreferences : private om::SingletonBase<Om3dPreferences> {
 private:
  int mCrosshairValue_;
  bool m2DViewFrameIn3D_;
  bool mDrawCrosshairsIn3D_;
  bool mDoDiscoBall;
  bool m2DViewPaneIn3D_;
  double scrollRate_;

 public:
  static uint32_t getCrosshairValue() { return instance().mCrosshairValue_; }
  static void setCrosshairValue(const uint32_t value) {
    instance().mCrosshairValue_ = value;
    LocalPrefFiles::writeSettingNumber<uint32_t>("crosshairValue", value);
  }

  static bool get2DViewFrameIn3D() { return instance().m2DViewFrameIn3D_; }
  static void set2DViewFrameIn3D(const bool value) {
    instance().m2DViewFrameIn3D_ = value;
    LocalPrefFiles::writeSettingBool("2DViewFrameIn3D", value);
  }

  static bool get2DViewPaneIn3D() { return instance().m2DViewPaneIn3D_; }
  static void set2DViewPaneIn3D(const bool value) {
    instance().m2DViewPaneIn3D_ = value;
    LocalPrefFiles::writeSettingBool("2DViewPaneIn3D", value);
  }

  static bool getDrawCrosshairsIn3D() {
    return instance().mDrawCrosshairsIn3D_;
  }
  static void setDrawCrosshairsIn3D(const bool value) {
    instance().mDrawCrosshairsIn3D_ = value;
    LocalPrefFiles::writeSettingBool("DrawCrosshairsIn3D", value);
  }

  static bool getDoDiscoBall() { return instance().mDoDiscoBall; }
  static void setDoDiscoBall(const bool value) {
    instance().mDoDiscoBall = value;
    LocalPrefFiles::writeSettingBool("DoDiscoBall", value);
  }

  static double getScrollRate() { return instance().scrollRate_; }
  static void setScrollRate(const double val) {
    instance().scrollRate_ = val;
    LocalPrefFiles::writeSettingNumber<double>("3DScrollRate", val);
  }

 private:
  Om3dPreferences() {
    setDefaultCrosshairValue();
    setDefault2DViewFrameIn3D();
    setDefaultDrawCrosshairsIn3D();
    setDefault2DViewPaneIn3D();
    setDefaultScrollRate();
  }
  ~Om3dPreferences() {}

  void setDefaultCrosshairValue() {
    const uint32_t defaultRet = 1;
    mCrosshairValue_ = LocalPrefFiles::readSettingNumber<uint32_t>(
        "crosshairValue", defaultRet);
  }

  void setDefault2DViewFrameIn3D() {
    const bool defaultRet = false;
    m2DViewFrameIn3D_ =
        LocalPrefFiles::readSettingBool("2DViewFrameIn3D", defaultRet);
  }

  void setDefaultDrawCrosshairsIn3D() {
    const bool defaultRet = false;
    mDrawCrosshairsIn3D_ =
        LocalPrefFiles::readSettingBool("DrawCrosshairsIn3D", defaultRet);
  }

  void setDefault2DViewPaneIn3D() {
    const bool defaultRet = false;
    m2DViewPaneIn3D_ =
        LocalPrefFiles::readSettingBool("2DViewPaneIn3D", defaultRet);
  }

  void setDefaultScrollRate() {
    const int defaultVal = 3;
    scrollRate_ =
        LocalPrefFiles::readSettingNumber<double>("3DScrollRate", defaultVal);
  }

  friend class zi::singleton<Om3dPreferences>;
};
