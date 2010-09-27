#ifndef STRING_HELPERS_H
#define STRING_HELPERS_H

#include "common/omCommon.h"
#include <QStringList>
#include <zi/utility/for_each.hpp>

class StringHelpers
{
 public:
	static QString getStringFromSegmentSet( const OmSegIDsSet & data_set );
	static QString getStringFromIDset( const OmIDsSet & data_set );
	static QString getStringFromStringList( const QStringList & data_set );

	template <typename T>
	static std::string commaDeliminateNum(const T num){
		const std::string rawNumAsStr = QString::number(num).toStdString();

		size_t counter = 0;
		QString ret;
		FOR_EACH_R(i, rawNumAsStr){
			++counter;
			ret.prepend( (*i) );
			if( 0 == ( counter % 3 ) &&
				counter != rawNumAsStr.size() ){
				ret.prepend(',');
			}
		}

		return ret.toStdString();
	}

	template <typename T>
	static QString commaDeliminateNumQT(const T num){
		return QString::fromStdString(commaDeliminateNum(num));
	}

	static unsigned int getUInt(const QString& arg)
	{
		bool ok;
		unsigned int ret = arg.toUInt(&ok, 10);
		if( ok ){
			return ret;
		} else {
			printf("could not parse to unsigned int \"%s\"\n", qPrintable(arg) );
			return 0;
		}
	}

	static double getDouble(const QString& arg)
	{
		bool ok;
		double ret = arg.toDouble(&ok);
		if( ok ){
			return ret;
		} else {
			printf("could not parse to double  \"%s\"\n", qPrintable(arg) );
			return 0;
		}
	}

	static float getFloat(const QString& arg)
	{
		bool ok;
		double ret = arg.toFloat(&ok);
		if( ok ){
			return ret;
		} else {
			printf("could not parse to float  \"%s\"\n", qPrintable(arg) );
			return 0;
		}
	}
};

#endif
