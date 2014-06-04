#pragma once

#include "precomp.h"
#include "network/http/curlHandle.hpp"
#include "threads/waitable.hpp"
#include "threads/continuable.hpp"

namespace om {
namespace network {

class CURLCore;

class Request : public thread::Waitable {
 public:
  Request();

  enum class State {
    PENDING,
    DOING,
    DONE,
    ABORTED
  };

 protected:
  ~Request();

  explicit operator bool() { return (bool)lease_; }

  virtual void SetCurlOptions(CURL*) = 0;
  virtual void Finish(CURL*) {}

 private:
  void getResponseCode();
  void start(handle_pool::Lease&& l);
  void finish();
  void abort();

  static void ResetHandle(std::shared_ptr<handle> h) {
    if ((bool)h) {
      curl_multi_remove_handle(h->Handle);
      curl_easy_reset(h->Handle);
    }
  }

  handle_pool::Lease lease_;
  friend class CURLCore;

  PROP(long, returnCode);
  PROP(State, state);
};

class CURLCore {
 public:
  CURLCore();
  ~CURLCore();

  template <typename TReq, typename... TArgs>
  std::enable_if<std::is_base_of<Request, TReq>::value,
                 std::shared_ptr<TReq>>::type
  CreateRequest(TArgs&& args...) {
    auto new_req = std::make_shared<TReq>(std::forward<TArgs>(args)...);
    {
      std::lock_guard<std::mutex>(pending_mutex_);
      pending_.push_back(std::weak_ptr<TReq>(new_req));
    }
    return new_req;
  }

 private:
  void watch();
  void start_pending();
  int check_handles();
  timeval find_timeout();
  void handle_messages();

  CURLM* handle_;
  std::atomic<bool> killing_;
  const int max_active_requests_;

  std::mutex pending_mutex_;
  std::deque<std::weak_ptr<Request>> pending_;

  handle_pool pool_;
  std::unordered_map<CURL*, std::weak_ptr<Request>> active_;

  std::thread watcherThread_;
};
}
}  // namespace om::network::