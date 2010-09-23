#ifndef SORT_HELPERS_H
#define SORT_HELPERS_H

#include <strnatcmp.h>
#include <QFileInfoList>
#include <QFileInfo>
#include <QStringList>

class SortHelpers {
public:
	static QStringList SortNaturally(const QStringList& in)
	{
		QStringList retVal = in;
		qSort(retVal.begin(),
		      retVal.end(),
		      NaturalStringCaseInsensitiveCompareLessThan);
		return retVal;
	}

	static QFileInfoList SortNaturally(const QFileInfoList& in)
	{
		QFileInfoList retVal = in;
		qSort(retVal.begin(),
		      retVal.end(),
		      NaturalStringCaseInsensitiveCompareLessThanQFile);
		return retVal;
	}

private:

	static bool NaturalStringCaseInsensitiveCompareLessThan(const QString& lhs,
								const QString& rhs)
	{
		return strnatcmp(qPrintable(lhs),
				 qPrintable(rhs)) < 0;
	}

	static bool NaturalStringCaseInsensitiveCompareLessThanQFile(const QFileInfo& lhs,
								     const QFileInfo & rhs)
	{
		return NaturalStringCaseInsensitiveCompareLessThan(lhs.fileName(),
								   rhs.fileName());
	}
};

#endif
