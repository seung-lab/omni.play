#ifndef OM_2D_PREFERENCES_HPP
#define OM_2D_PREFERENCES_HPP

#include "common/omCommon.h"
#include "zi/omUtility.h"
#include "utility/localPrefFiles.h"

class Om2dPreferences : private om::singletonBase<Om2dPreferences> {
private:
    bool haveAlphaGoToBlack_;
    bool showView2dCrosshairs_;
    int crosshairHoleSize_;

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

private:
    Om2dPreferences()
    {
        setDefaultHaveAlphaGoToBlack();
        setDefaultShowCrosshairs();
        setDefaultCrosshairHoleSize();
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

    friend class zi::singleton<Om2dPreferences>;
};

#endif
