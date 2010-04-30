#ifndef STRING_HELPERS_H
#define STRING_HELPERS_H

#include "common/omCommon.h"
#include <QStringList>

class StringHelpers 
{
 public:
	static QString getStringFromSegmentSet( const SegmentDataSet & data_set );
	static QString getStringFromIDset( const OmIds & data_set );
	static QString getStringFromStringList( const QStringList & data_set );
	static QString commaDeliminateNumber( const int num );
	static unsigned int getUInt( QString arg );
	static double getDouble( QString arg );
};

#endif
