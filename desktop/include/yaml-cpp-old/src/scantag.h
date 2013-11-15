#ifndef SCANTAG_H_62B23520_7C8E_11DE_8A39_0800200C9A66_OLD
#define SCANTAG_H_62B23520_7C8E_11DE_8A39_0800200C9A66_OLD

#if !defined(__GNUC__) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || (__GNUC__ >= 4) // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif


#include <string>
#include "stream.h"

namespace YAMLold
{
	const std::string ScanVerbatimTag(Stream& INPUT);
	const std::string ScanTagHandle(Stream& INPUT, bool& canBeHandle);
	const std::string ScanTagSuffix(Stream& INPUT);
}

#endif // SCANTAG_H_62B23520_7C8E_11DE_8A39_0800200C9A66_OLD

