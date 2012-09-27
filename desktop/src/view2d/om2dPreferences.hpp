#pragma once

#include "common/omCommon.h"
#include "zi/omUtility.h"
#include "utility/localPrefFiles.h"

class Om2dPreferences : private om::singletonBase<Om2dPreferences> {
private:
    bool haveAlphaGoToBlack_;
    bool showView2dCrosshairs_;
    int crosshairHoleSize_;
    double mipRate_;

public:
// have alpha fade to black
    static bool HaveAlphaGoToBlack(){
        return instance().haveAlphaGoToBlack_;
    }
    static void HaveAlphaGoToBlack(const bool value)
    {
        instance().haveAlphaGoToBlack_ = value;
        LocalPrefFiles::writeSettingBool("haveAlphaGoToBlack", value);
    }

// show crosshairs
    static bool ShowCrosshairs(){
        return instance().showView2dCrosshairs_;
    }
    static void ShowCrosshairs(const bool val)
    {
        instance().showView2dCrosshairs_ = val;
        LocalPrefFiles::writeSettingBool("ShowView2dCrosshairs", val);
    }

// cross hair opening size
    static int CrosshairHoleSize(){
        return instance().crosshairHoleSize_;
    }
    static void CrosshairHoleSize(const int val)
    {
        instance().crosshairHoleSize_ = val;
        LocalPrefFiles::writeSettingNumber<int32_t>("View2dCrosshairHoleSize", val);
    }

// cross hair opening size
    static double MipRate(){
        return instance().mipRate_;
    }
    static void MipRate(const double val)
    {
        instance().mipRate_ = val;
        LocalPrefFiles::writeSettingNumber<double>("MipRate", val);
    }


private:
    Om2dPreferences()
    {
        setDefaultHaveAlphaGoToBlack();
        setDefaultShowCrosshairs();
        setDefaultCrosshairHoleSize();
        setDefaultMipRate();
    }
    ~Om2dPreferences(){}

    void setDefaultHaveAlphaGoToBlack()
    {
        const bool defaultRet = true;
        haveAlphaGoToBlack_ =
            LocalPrefFiles::readSettingBool("haveAlphaGoToBlack", defaultRet);
    }

    void setDefaultShowCrosshairs()
    {
        const bool defaultVal = true;
        showView2dCrosshairs_ =
            LocalPrefFiles::readSettingBool("ShowView2dCrosshairs", defaultVal);
    }

    void setDefaultCrosshairHoleSize()
    {
        const int defaultVal = 20;
        crosshairHoleSize_ =
            LocalPrefFiles::readSettingNumber<int32_t>("View2dCrosshairHoleSize",
                                                       defaultVal);
    }

    void setDefaultMipRate()
    {
        const int defaultVal = 3;
        mipRate_ =
            LocalPrefFiles::readSettingNumber<double>("MipRate",
                                                       defaultVal);
    }

    friend class zi::singleton<Om2dPreferences>;
};

