/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef _ZI_ZARGS_ZARGS_H_
#define _ZI_ZARGS_ZARGS_H_

#include <vector>
#include <string>
#include <list>
#include <exception>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <inttypes.h>
#include <boost/lexical_cast.hpp>
#include "zi/base/base.h"


namespace zi { namespace Args {

inline std::string toLower(const std::string& s) {
  size_t len = s.size();
  std::string ret = s;
  for (size_t i = 0; i < len; ++i )
    ret[i] = static_cast<char>(tolower(s[i]));
  return ret;
}

inline std::string stripQuotes(const std::string& s) {
  size_t len = s.size();
  if (len < 2) return s;
  if ((s[0] == '"'  && s[len-1] == '"') ||
      (s[0] == '\'' && s[len-1] == '\'')) {
    std::string ret(s.begin()+1, s.end()-1);
    return ret;
  }
  return s;
}

inline bool beginsWith(const std::string &b, const std::string &s) {
  if (s.size() < b.size()) return false;
  return (b == s.substr(0, b.size()));
}

inline void explode(std::vector<std::string> &ret,
                    const std::string& source,
                    char splitter = ' ')
{
  size_t startpos;
  size_t pos  = 0;
  size_t npos = std::string::npos;
  std::string tmp;
  std::string src = source;

  size_t length = src.size();
  while (pos != npos && pos < length) {
    startpos = pos;
    pos = src.find_first_of(splitter, pos);
    if (pos != 0) {
      tmp = source.substr(startpos, pos-startpos);
      ret.push_back(tmp);
      if (pos != npos)
        ++pos;
    }
    else
      break;
  }
}


class ArgsException : public std::exception {
public:
  ArgsException(): what_("Default ziArgsException") {}
  ArgsException(const std::string& w): what_(w)     {}
  virtual ~ArgsException() throw() {}
  virtual const char* what() const throw() { return what_.c_str(); }
  virtual const std::string& str() const throw() { return what_;   }
protected:
  std::string what_;
};

template <class T> class ArgParser {
public:
  virtual ~ArgParser() {}
  virtual bool parseInto(T* target, const std::string &str) {
    try {
      *target = boost::lexical_cast<T>(str);
      return true;
    } catch (...) {
      return false;
    }
  }
};

template <> class ArgParser<bool> {
public:
  bool parseInto(bool* target, const std::string &str) {
    try {
      *target = boost::lexical_cast<bool>(str);
      return true;
    } catch (...) {
      std::string s(toLower(str));
      if (s == "t" || s == "true" || s == "yes" || s == "y") {
        *target = true;
        return true;
      }
      if (s == "f" || s == "false" || s == "no" || s == "n") {
        *target = false;
        return true;
      }
      return false;
    }
  }
};

template <> class ArgParser<std::string> {
public:
  bool parseInto(std::string* target, const std::string &str) {
    *target = std::string(str);
    return true;
  }
};

template <class T> class ArgMatcherBase {
public:
  virtual ~ArgMatcherBase() {}
  virtual bool match(const std::string &argName, std::list<std::string> &q) {
    if (q.empty()) return false;
    std::string s = q.front();
    if (s == ("-" + argName) || s == ("--" + argName)) {
      q.pop_front();
      return true;
    }
    if (beginsWith("-" + argName + "=", s) ||
        beginsWith("--" + argName + "=", s)) {
      std::string all = q.front();
      size_t eqPos = all.find_first_of('=', 0);
      q.pop_front();
      q.push_front(all.substr(eqPos + 1, all.size() - eqPos - 1));
      return true;
    }
    return false;
  }
};

template <class T> class ArgMatcher: public ArgMatcherBase<T> {};

template <> class ArgMatcher<bool>: public ArgMatcherBase<bool> {
public:
  bool match(const std::string &argName, std::list<std::string> &q) {
    std::string s = q.front();
    if (s == ("-" + argName) || s == ("--" + argName)) {
      q.pop_front();
      q.push_front("1");
      return true;
    }
    if (s == ("-no" + argName) || s == ("--no" + argName)) {
      q.pop_front();
      q.push_front("0");
      return true;
    }
    return ArgMatcherBase<bool>::match(argName, q);
  }
};

class ZiArgHandler {
public:
  virtual ~ZiArgHandler() {}
  virtual std::string getName()         const = 0;
  virtual std::string getType()         const = 0;
  virtual std::string getDefault()      const = 0;
  virtual std::string getDescription()  const = 0;
  virtual bool parse(std::list<std::string> &q)  = 0;
};

class ZiArgHandlerFactory {
public:
  virtual ZiArgHandler* getHandler() = 0;
  virtual ~ZiArgHandlerFactory() {}
};

template<class T> class ZiArgHandlerFactoryT : public ZiArgHandlerFactory {
public:
  virtual ZiArgHandler* getHandler() { return new T(); }
};

class ZiArguments : private CantBeCopied {
public:
  static ZiArguments& get();
  void registerHandler(ZiArgHandlerFactory* f) {
    handlerFactories_.push_back(f);
  }
  template<class T> struct Signup {
    Signup() {
      ZiArguments::get().registerHandler(new ZiArgHandlerFactoryT<T>);
    }
  };
  void run();
  bool parseArgs(int &argc, char **argv, bool removeArgs = true);
  static void ParseArgs(int &argc, char **argv, bool removeArgs = true);
private:
  std::vector<ZiArgHandlerFactory*> handlerFactories_;
  std::string         fileName_;
  static ZiArguments* args_;
};


#define ZiARG_DEFINITION(_name, _default, _description, _type, _typeS)  \
  static _type ZiArg_ ## _name = _default;                              \
  class __ArgHandler_ ## _name : public zi::Args::ZiArgHandler {        \
  private:                                                              \
  zi::Args::ArgMatcher<_type> matcher_;                                 \
  zi::Args::ArgParser<_type>  parser_ ;                                 \
  public:                                                               \
  std::string getName()        const { return #_name;        }          \
  std::string getType()        const { return #_typeS;       }          \
  std::string getDefault()     const { return #_default;     }          \
  std::string getDescription() const { return _description; }           \
  bool parse(std::list<std::string> &q) {                               \
    if (matcher_.match( #_name, q)) {                                   \
      std::string val = zi::Args::stripQuotes(q.front());               \
      if (!parser_.parseInto(&ZiArg_ ## _name, val)) {                  \
        throw new zi::Args::ArgsException                               \
          (getName() + " [" + getType() + "] can't be parsed from" +    \
           " \"" + val + "\"");                                         \
      }                                                                 \
      return true;                                                      \
    }                                                                   \
    return false;                                                       \
  }                                                                     \
  };                                                                    \
  static zi::Args::ZiArguments::Signup<__ArgHandler_ ## _name>          \
  signup_ ## _name ## _instance


#define ZiARG_int32(_name, _default, _description)                      \
  ZiARG_DEFINITION(_name, _default, _description, int32_t, INT32)
#define ZiARG_uint32(_name, _default, _description)                     \
  ZiARG_DEFINITION(_name, _default, _description, uint32_t, UINT32)
#define ZiARG_int64(_name, _default, _description)                      \
  ZiARG_DEFINITION(_name, _default, _description, int64_t, INT64)
#define ZiARG_uint64(_name, _default, _description)                     \
  ZiARG_DEFINITION(_name, _default, _description, uint64_t, INT64)
#define ZiARG_float(_name, _default, _description)              \
  ZiARG_DEFINITION(_name, _default, _description, float, FLOAT)
#define ZiARG_double(_name, _default, _description)                     \
  ZiARG_DEFINITION(_name, _default, _description, double, DOUBLE)
#define ZiARG_bool(_name, _default, _description)               \
  ZiARG_DEFINITION(_name, _default, _description, bool, BOOLEAN)
#define ZiARG_string(_name, _default, _description)                     \
  ZiARG_DEFINITION(_name, _default, _description, std::string, STRING)

#define DECLARE_ZiARG_int32(_name)  extern int32_t     ZiArg_ ## _name
#define DECLARE_ZiARG_uint32(_name) extern uint32_t    ZiArg_ ## _name
#define DECLARE_ZiARG_int64(_name)  extern int64_t     ZiArg_ ## _name
#define DECLARE_ZiARG_uint64(_name) extern uint64_t    ZiArg_ ## _name
#define DECLARE_ZiARG_float(_name)  extern float       ZiArg_ ## _name
#define DECLARE_ZiARG_double(_name) extern double      ZiArg_ ## _name
#define DECLARE_ZiARG_string(_name) extern std::string ZiArg_ ## _name
#define DECLARE_ZiARG_bool(_name)   extern bool        ZiArg_ ## _name

} }

#endif
