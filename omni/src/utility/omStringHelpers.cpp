#include "utility/omStringHelpers.h"

static const int numOfItemsToAddBeforeLinebreak = 8;

QString OmStringHelpers::getStringFromSegmentSet( const OmSegIDsSet & data_set )
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

QString OmStringHelpers::getStringFromIDset( const OmIDsSet & data_set )
{
	if( data_set.size() == 0 ){
		return "";
	}

	QStringList strs;
	foreach( OmID val, data_set ){
		strs << QString::number( val );
	}

	return getStringFromStringList( strs );
}

QString OmStringHelpers::getStringFromStringList( const QStringList & data_set )
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
