#ifndef STRING_HELPERS_H
#define STRING_HELPERS_H

#include "common/omCommon.h"
#include <QStringList>

class StringHelpers
{
 public:
	static QString getStringFromSegmentSet( const OmSegIDsSet & data_set );
	static QString getStringFromIDset( const OmIDsSet & data_set );
	static QString getStringFromStringList( const QStringList & data_set );
	static QString commaDeliminateNumber( const long long num );

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
