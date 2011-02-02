#ifndef SORT_HELPERS_H
#define SORT_HELPERS_H

#include <strnatcmp.h>
#include <QFileInfoList>
#include <QFileInfo>
#include <QStringList>

class SortHelpers {
public:
	static void SortNaturally(std::vector<QFileInfo>& in)
	{
		std::sort(in.begin(),
				  in.end(),
				  naturalStringCaseInsensitiveCompareLessThanQFile);
	}

	static QStringList SortNaturally(const QStringList& in)
	{
		QStringList retVal = in;
		qSort(retVal.begin(),
		      retVal.end(),
		      naturalStringCaseInsensitiveCompareLessThan);
		return retVal;
	}

	static QFileInfoList SortNaturally(const QFileInfoList& in)
	{
		QFileInfoList retVal = in;
		qSort(retVal.begin(),
		      retVal.end(),
		      naturalStringCaseInsensitiveCompareLessThanQFile);
		return retVal;
	}

private:

	static bool naturalStringCaseInsensitiveCompareLessThan(const QString& lhs,
															const QString& rhs)
	{
		return strnatcmp(qPrintable(lhs),
						 qPrintable(rhs)) < 0;
	}

	static bool naturalStringCaseInsensitiveCompareLessThanQFile(const QFileInfo& lhs,
																 const QFileInfo & rhs)
	{
		return naturalStringCaseInsensitiveCompareLessThan(lhs.fileName(),
														   rhs.fileName());
	}
};

#endif
