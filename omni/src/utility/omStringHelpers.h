#ifndef STRING_HELPERS_H
#define STRING_HELPERS_H

#include "common/omCommon.h"
#include "common/omString.hpp"
#include "zi/omUtility.h"

class OmStringHelpers{
public:

    template <typename T>
    inline static QString CommaDeliminateNumQT(const T num){
        return QString::fromStdString(CommaDeliminateNum(num));
    }

    template <typename T>
    inline static std::string CommaDeliminateNum(const T num)
    {
        const std::string rawNumAsStr = om::string::num(num);

        size_t counter = 0;
        std::string ret;

        FOR_EACH_R(i, rawNumAsStr){
            ++counter;
            ret += *i;
            if( 0 == ( counter % 3 ) &&
                counter != rawNumAsStr.size() )
            {
                ret += ',';
            }
        }

        std::reverse(ret.begin(), ret.end());
        return ret;
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

#endif
