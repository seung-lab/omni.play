#ifndef SORT_HELPERS_H
#define SORT_HELPERS_H

#include "common/omDebug.h"
#include <strnatcmp.h>
#include <QFileInfoList>
#include <QFileInfo>
#include <QStringList>

class SortHelpers
{
 public:
	static QFileInfoList sortNaturally( QFileInfoList in );
	static QStringList sortNaturally( QStringList in );

 private:
	static bool NaturalStringCaseInsensitiveCompareLessThan( const QString & lhsQ, const QString & rhsQ );
	static bool NaturalStringCaseInsensitiveCompareLessThanQFile( const QFileInfo & lhsQ, const QFileInfo & rhsQ );
};

#endif
