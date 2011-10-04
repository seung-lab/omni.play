#pragma once

#include "common/omCommon.h"
#include "common/omString.hpp"
#include "zi/omUtility.h"

class OmStringHelpers{
public:

    template <typename T>
    inline static QString HumanizeNumQT(const T num, const char sep = ','){
        return QString::fromStdString(om::string::humanizeNum(num, sep));
    }

    static uint32_t getUInt(const QString& arg)
    {
        bool ok;
        uint32_t ret = arg.toUInt(&ok, 10);
        if(ok){
            return ret;
        }
        throw OmIoException("could not parse to uint32_t", arg);
    }

    static bool getBool(const QString& arg)
    {
        if("true" == arg){
            return true;
        }
        if("false" == arg){
            return false;
        }

        return getUInt(arg);
    }

    static double getDouble(const QString& arg)
    {
        bool ok;
        double ret = arg.toDouble(&ok);
        if( ok ){
            return ret;
        }
        throw OmIoException("could not parse to double", arg);
    }

    static float getFloat(const QString& arg)
    {
        bool ok;
        float ret = arg.toFloat(&ok);
        if( ok ){
            return ret;
        }
        throw OmIoException("could not parse to float", arg);
    }
};

