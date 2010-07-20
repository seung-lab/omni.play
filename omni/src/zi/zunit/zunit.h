/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef _ZI_ZUINT_ZUINT_H_
#define _ZI_ZUNIT_ZUINT_H_

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <inttypes.h>
#include "zi/base/base.h"
#include "zi/base/bash.h"


namespace zi { namespace Test {

class UnitTest : private CantBeCopied {
public:
  UnitTest() : passed_(true) {};
  virtual ~UnitTest() {};

  virtual const char*        getName() const = 0;
  virtual const char*        getFile() const = 0;
  virtual void run() = 0;

  virtual const std::string& getRet()  { return errors_; }
  virtual bool getPassed()  { return passed_; }
protected:
  bool        passed_;
  int         noPassed_, noFailed_;
  std::string errors_;
};

class UnitTestFactory {
public:
  virtual UnitTest* getTest() = 0;
  virtual ~UnitTestFactory() {}
};

template<class T> class UnitTestFactoryT : public UnitTestFactory {
public:
  virtual UnitTest* getTest() { return new T(); }
};

class UnitTestSuite : private CantBeCopied {
public:
  static UnitTestSuite& get();
  void registerFactory(UnitTestFactory* f) { factories_.push_back(f); }
  int  runTests();
  template<class T> struct Signup {
    Signup() { UnitTestSuite::get().registerFactory(new UnitTestFactoryT<T>); }
  };
private:
  UnitTestSuite() {}
  ~UnitTestSuite();
  std::vector<UnitTestFactory*> factories_;
  static UnitTestSuite* suite_;
};

#define __STRINGIFYP(x) #x
#define __STRINGIFY(x) __STRINGIFYP(x)

#define EXPECT_TRUE(_val)                                       \
  do {                                                          \
    if (!(_val)) {                                              \
      errors_ = "EXPECT_TRUE(" #_val ") FAILED: "               \
        "file: "  __FILE__  ", "  __STRINGIFY(__LINE__);        \
      passed_ = false;                                          \
    }                                                           \
  } while(0)

#define EXPECT_FALSE(_val)                                      \
  do {                                                          \
    if ((_val)) {                                               \
      errors_ = "EXPECT_FALSE(" #_val ") FAILED: "              \
        "file: "  __FILE__  ", "  __STRINGIFY(__LINE__);        \
      passed_ = false;                                          \
    }                                                           \
  } while(0)

#define EXPECT_BINARY(_WHAT, _OPERATOR, _left, _right)          \
  do {                                                          \
    if (!(_left _OPERATOR _right)) {                            \
      errors_ = "EXPECT_" #_WHAT "(" #_left                     \
        ", " #_right ") FAILED: "                               \
        "file: "  __FILE__  ", "  __STRINGIFY(__LINE__);        \
      passed_ = false;                                          \
    }                                                           \
  } while(0)

#define EXPECT_EQ( _left, _right) EXPECT_BINARY(EQ,  ==, _left, _right)
#define EXPECT_NEQ(_left, _right) EXPECT_BINARY(NEQ, !=, _left, _right)
#define EXPECT_GT( _left, _right) EXPECT_BINARY(GT,  > , _left, _right)
#define EXPECT_GTE(_left, _right) EXPECT_BINARY(GTE, >=, _left, _right)
#define EXPECT_LT( _left, _right) EXPECT_BINARY(LT,  < , _left, _right)
#define EXPECT_LTE(_left, _right) EXPECT_BINARY(LTE, <=, _left, _right)

#define ZUNIT_TEST(_NAME)                                               \
  class UnitTest_ ## _NAME : public zi::Test::UnitTest {                \
  public:                                                               \
  virtual void run();                                                   \
  virtual const char* getName() const { return #_NAME;    }             \
  virtual const char* getFile() const { return __FILE__;  }             \
  };                                                                    \
  static zi::Test::UnitTestSuite::Signup<UnitTest_ ## _NAME>            \
  signup_ ## _NAME ## _Test;                                            \
  void UnitTest_ ## _NAME ::run()

#define RUN_ALL_TESTS() (zi::Test::UnitTestSuite::get().runTests())

} }

#endif
