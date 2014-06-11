#pragma once

#include <functional>
#include "events/events.h"

namespace om {

// STOLEN from here:
// http://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda
template <typename T>
struct function_traits : public function_traits<decltype(&T::operator())> {};
// For generic types, directly use the result of the signature of its
// 'operator()'

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...) const>
    // we specialize for pointers to member function
    {
  enum {
    arity = sizeof...(Args)
  };
  // arity is the number of arguments.

  typedef ReturnType result_type;

  template <size_t i>
  struct arg {
    typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    // the i-th argument is equivalent to the i-th tuple element of a tuple
    // composed of those arguments.
  };
};

template <typename Func, typename Enable = void>
class execer {
 public:
  constexpr static size_t arity = function_traits<Func>::arity;
  typedef typename function_traits<Func>::template arg<0>::type arg;
  execer(Func f) : func_(f) {}

  void operator()(arg v) {
    auto f = std::bind(func_, v);
    om::event::ExecuteOnMain(f);
  }

  Func func_;
};

template <typename Func>
class execer<Func,
             typename std::enable_if<function_traits<Func>::arity == 0>::type> {
 public:
  execer(Func f) : func_(f) {}

  void operator()() { om::event::ExecuteOnMain(func_); }

  Func func_;
};

template <typename Func>
execer<Func> exec(Func f) {
  return execer<Func>(f);
}

namespace thread {
template <typename T>
thread::Continuable<T>& operator>>=(thread::Continuable<T>& c,
                                    std::function<void(T)> f) {
  c.AddContinuation(exec(f));
  return c;
}
}
namespace network {
namespace http {
GetRequest& operator>>=(GetRequest& c,
                        typename thread::Continuable<
                            const boost::optional<std::string>&>::func_t f) {
  c.AddContinuation(exec(f));
  return c;
}

template <typename T>
TypedGetRequest<T>& operator>>=(
    TypedGetRequest<T>& c,
    typename thread::Continuable<const boost::optional<T>&>::func_t f) {
  c.AddContinuation(exec(f));
  return c;
}
}
}
}