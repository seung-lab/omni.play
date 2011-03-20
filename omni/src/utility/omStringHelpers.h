#ifndef STRING_HELPERS_H
#define STRING_HELPERS_H

#include "common/omCommon.h"
#include "zi/omUtility.h"

#include <QStringList>

class OmStringHelpers{
public:
	static QString getStringFromSegmentSet( const OmSegIDsSet & data_set );
	static QString getStringFromIDset( const OmIDsSet & data_set );
	static QString getStringFromStringList( const QStringList & data_set );

	template <typename T>
	inline static std::string CommaDeliminateNum(const T num){
		return CommaDeliminateNumQT(num).toStdString();
	}

	template <typename T>
	inline static QString CommaDeliminateNumQT(const T num)
	{
		const std::string rawNumAsStr = QString::number(num).toStdString();

		size_t counter = 0;
		QString ret;
		FOR_EACH_R(i, rawNumAsStr){
			++counter;
			ret.prepend( (*i) );
			if( 0 == ( counter % 3 ) &&
				counter != rawNumAsStr.size() )
			{
				ret.prepend(',');
			}
		}

		return ret;
	}

	static unsigned int getUInt(const QString& arg)
	{
		bool ok;
		unsigned int ret = arg.toUInt(&ok, 10);
		if( ok ){
			return ret;
		}
		throw OmIoException("could not parse to unsigned int", arg);
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
		double ret = arg.toFloat(&ok);
		if( ok ){
			return ret;
		}
		throw OmIoException("could not parse to float", arg);
	}
};

#endif
