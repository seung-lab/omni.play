/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef ZI_BASE_STRINGS_H_
#define ZI_BASE_STRINGS_H_

#include <string>
#include <sstream>
#include "zi/base/base.h"

namespace StringUtil {

template<typename T>
inline std::string toString(T t) {
  std::ostringstream oss;
  oss << t;
  return oss.str();
}

template<typename T>
inline T fromString(const std::string &s) {
  T t;
  std::istringstream iss(s);
  iss >> t;
  return t;
}

inline std::string toLower(const std::string& s) {
  size_t len = s.size();
  std::string ret = s;
  if ( len == 0 )
    return ret;
  for (size_t i = 0; i < len; ++i ) {
    ret[i] = static_cast<char>(tolower(s[i]));
  }
  return ret;
}

inline std::string toUpper(const std::string& s) {
  size_t len = s.size();
  std::string ret = s;
  if ( len == 0 )
    return ret;
  for (size_t i = 0; i < len; ++i ) {
    ret[i] = static_cast<char>(toupper(s[i]));
  }
  return ret;
}

inline std::string trim(const std::string& s) {

  std::string ret = s;

  std::string::iterator it = ret.begin();
  for( ; it != ret.end(); ++it ) {
    if(!isspace(*it))
      break;
  }

  ret.erase(ret.begin(), it);

  std::string::reverse_iterator rit = ret.rbegin();
  for( ; rit != ret.rend(); ++rit ) {
    if(!isspace(*rit))
      break;
  }

  std::string::difference_type dt = ret.rend() - rit;
  ret.erase(ret.begin() + dt, ret.end());

  return ret;
}

template<typename T>
void explode(T& ret, const std::string& source, char splitter = ' ') {
  size_t pos = 0;
  size_t startpos;
  size_t npos = std::string::npos;
  std::string tmp;

  std::string src = trim(source);

  size_t length = src.size();

  while (pos != npos && pos < length) {
    startpos = pos;
    pos = src.find_first_of(splitter, pos);
    if (pos != 0) {
      tmp = source.substr(startpos, pos-startpos);
      if (trim( tmp ).size())
        ret.push_back(tmp);
      if (pos != npos)
        ++pos;
    }
    else
      break;
  }
}

template<typename T>
std::string implode(const T& srcs, const std::string &splitter = "") {

  std::ostringstream oss;

  bool first = true;
  FOR_EACH (it, srcs) {
    if (!first)
      oss << splitter;
    oss << *it;
    first = false;
  }

  return oss.str();
}

}

#endif
