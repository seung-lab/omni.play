#include "utility/stringHelpers.h"

static const int numOfItemsToAddBeforeLinebreak = 8;

QString StringHelpers::getStringFromSegmentSet( const OmSegIDsSet & data_set )
{
	if( data_set.size() == 0 ){
		return "";
	}

	QStringList strs;
	foreach( OmSegID val, data_set ){
		strs << QString::number( val );
	}

	return getStringFromStringList( strs );
}

QString StringHelpers::getStringFromIDset( const OmIDsSet & data_set )
{
	if( data_set.size() == 0 ){
		return "";
	}

	QStringList strs;
	foreach( OmId val, data_set ){
		strs << QString::number( val );
	}

	return getStringFromStringList( strs );
}

QString StringHelpers::getStringFromStringList( const QStringList & data_set )
{
	if( data_set.size() == 0 ){
		return "";
	}

	QStringList strs = data_set;
	strs.sort();

	QString str;
	int counter = 0;

	foreach( QString val, strs ) {
		counter++;
		str += val;
		if( counter < data_set.size() ){
			str += ", ";
			if( counter > 0 &&
			    counter % numOfItemsToAddBeforeLinebreak == 0 ){
				str += "\n";
			}
		}
	}
	return str;
}

QString StringHelpers::commaDeliminateNumber( const long long num )
{
	QString rawNumAsStr = QString::number( num );

	QString str;
	QString::const_iterator i;
	int counter = 0;
	for (i = rawNumAsStr.end()-1; i != rawNumAsStr.begin()-1; i-- ){
		counter++;
		str.prepend( (*i) );
		if( 0 == ( counter % 3 ) &&
		    counter != rawNumAsStr.size() ){
			str.prepend(',');
		}
	}

	return str;
}
