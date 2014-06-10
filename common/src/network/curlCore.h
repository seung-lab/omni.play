#pragma once

#include "precomp.h"
#include "common/macro.hpp"
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

  ~Request();

  // Did we complete successfully.
  explicit operator bool() { return state_ == State::DONE; }

  void Detach() { detachedPtr_ = selfPtr_.lock(); }

 protected:
  virtual void SetCurlOptions(CURL*) = 0;
  virtual void Finish(CURL*) {}

 private:
  void getResponseCode();
  void start(handle_pool::Lease&& l);
  void finish();
  void abort();

  static void ResetHandle(CURLM* mh, std::shared_ptr<CurlHandle> h) {
    if ((bool)h) {
      curl_multi_remove_handle(mh, h->Handle);
      curl_easy_reset(h->Handle);
    }
  }

  handle_pool::Lease lease_;
  friend class CURLCore;

  PROP(long, returnCode);
  PROP(State, state);

  std::weak_ptr<Request> selfPtr_;
  std::shared_ptr<Request> detachedPtr_;
};

class CURLCore {
 public:
  CURLCore();
  ~CURLCore();

  template <typename TReq, typename... TArgs>
  typename std::enable_if<std::is_base_of<Request, TReq>::value,
                          std::shared_ptr<TReq>>::type
  CreateRequest(TArgs&&... args) {
    auto new_req = std::make_shared<TReq>(std::forward<TArgs>(args)...);
    new_req->selfPtr_ = std::weak_ptr<TReq>(new_req);
    {
      std::lock_guard<std::mutex> lk(pending_mutex_);
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