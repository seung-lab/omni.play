#include "sortHelpers.h"

bool SortHelpers::NaturalStringCaseInsensitiveCompareLessThan( const QString & lhsQ, const QString & rhsQ )
{
	std::string lhs = lhsQ.toStdString();
	std::string rhs = rhsQ.toStdString();
	return strnatcmp(lhs.c_str(), rhs.c_str()) < 0;
}

QStringList SortHelpers::sortNaturally( QStringList in )
{
	QStringList retVal = in;
	qSort( retVal.begin(), retVal.end(), NaturalStringCaseInsensitiveCompareLessThan );
	return retVal;
}

bool SortHelpers::NaturalStringCaseInsensitiveCompareLessThanQFile( const QFileInfo & lhsQ, const QFileInfo & rhsQ )
{
	std::string lhs = lhsQ.fileName().toStdString();
	std::string rhs = rhsQ.fileName().toStdString();
	return strnatcmp(lhs.c_str(), rhs.c_str()) < 0;
}

QFileInfoList SortHelpers::sortNaturally( QFileInfoList in )
{
	QFileInfoList retVal = in;
	qSort( retVal.begin(), retVal.end(), NaturalStringCaseInsensitiveCompareLessThanQFile );
	return retVal;
}
