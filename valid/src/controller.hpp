#pragma once

#include "zi/utility.h"
#include <vector>
#include <sstream>
#include <string>

namespace om {
namespace valid {

class Controller : public zi::enable_singleton_of_this<Controller> {
 public:
  class Context {
   public:
    Context(std::string text) : Text{text} { Controller::PushContext(this); }

    template <typename T>
    Context(T obj) {
      std::stringstream ss;
      ss << obj;
      Text = ss.str();
      Controller::PushContext(this);
    }

    ~Context() { Controller::PopContext(); }

    std::string Text;
    int Errors = 0;
  };

  static bool Test(bool result, std::string message) {
    if (!result) {
      instance().hasFailed_ = true;
      SayFail(message);
      return !instance().fail_;
    }
    return true;
  }

  template <typename T>
  static bool Test(bool result, T obj) {
    std::stringstream ss;
    ss << obj;
    return Test(result, ss.str());
  }

  static bool MultiTest(bool result, std::string message, int count) {
    if (!result) {
      instance().context_.back()->Errors++;
      if (instance().context_.back()->Errors == count) {
        instance().hasFailed_ = true;
        SayFail(message);
        return !instance().fail_;
      } else {
        Say(message, "Fail: ", instance().context_.back()->Errors);
      }
    }
    return true;
  }

  template <typename T>
  static bool MultiTest(bool result, T obj, int count) {
    std::stringstream ss;
    ss << obj;
    return MultiTest(result, ss.str(), count);
  }

  static void SetFail(bool fail) { instance().fail_ = fail; }

  static void SetQuiet(bool quiet) { instance().quiet_ = quiet; }

  static void SetSample(bool sample) { instance().sample_ = sample; }

  static bool HasFailed() { return instance().hasFailed_; }

  static void PushContext(Context* context) {
    instance().context_.push_back(context);
    Say();
  }

  static void PopContext() { instance().context_.pop_back(); }

  static int Step(int from, int to, int step = 1) {
    if (!instance().sample_) {
      return step;
    }
    int binned = (to - from) / 3;
    if (binned % step != 0) {
      binned += binned % step - step;
    }
    if (binned < 1) {
      return 1;
    }
    return binned;
  }

  template <typename... TArgs>
  static void Say(TArgs... args) {
    if (!instance().quiet_) {
      instance().print_context();
      say(args...);
      log_infos << std::endl;
    }
  }

  template <typename... TArgs>
  static void SayFail(TArgs... args) {
    if (!instance().quiet_ || instance().fail_) {
      say(args...);
      log_infos << ":";
    }
  }

  static void Reset() { instance().hasFailed_ = false; }

 private:
  Controller()
      : quiet_(false), fail_(false), hasFailed_(false), sample_(false) {}

  std::vector<Context*> context_;
  bool quiet_;
  bool fail_;
  bool hasFailed_;
  bool sample_;

  void print_context() const {
    for (int i = 0; i < context_.size(); ++i) {
      log_infos << "[" << context_[i]->Text << "]";
    }
    log_infos << ":";
  }

  template <typename T, typename... TArgs>
  static void say(T toSay, TArgs... args) {
    log_infos << toSay;
    say(args...);
  }
  template <typename T>
  static void say(T toSay) {
    log_infos << toSay;
  }
  static void say() {}

  friend class zi::singleton<Controller>;
};

#define VALID_TEST(test, message)         \
  if (!Controller::Test(test, message)) { \
    return false;                         \
  }

#define VALID_MULTITEST(test, message, count)         \
  if (!Controller::MultiTest(test, message, count)) { \
    return false;                                     \
  }

#define VALID_FOR(idx, from, to, step) \
  for (int idx = from; idx < to; idx += Controller::Step(from, to, step))
}
}  // namespace om::valid::