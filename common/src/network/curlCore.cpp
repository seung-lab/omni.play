#include "network/curlCore.h"

namespace om {
namespace network {

Request::Request() : Waitable(), returnCode_(0), state_(State::PENDING) {}

Request::~Request() {
  set_ready();  // Can you wait on an object which is being destroyed???
}

void Request::getResponseCode() {
  auto err =
      curl_easy_getinfo(lease_->Handle, CURLINFO_RESPONSE_CODE, &returnCode_);
  if (err) {
    log_debugs << "CURL Error with response: " << curl_easy_strerror(err);
    returnCode_ = -1;
  }
}

void Request::start(handle_pool::Lease&& l) {
  lease_ = std::move(l);
  SetCurlOptions(lease_->Handle);
  state_ = State::DOING;
}

void Request::finish() {
  getResponseCode();
  Finish(lease_->Handle);
  lease_.release();
  state_ = State::DONE;
  set_ready();
  detachedPtr_.reset();
}

void Request::abort() {
  lease_.release();
  state_ = State::ABORTED;
  set_ready();
  detachedPtr_.reset();
}

CURLCore::CURLCore()
    : handle_(curl_multi_init()),
      killing_(false),
      max_active_requests_(10),
      pool_(max_active_requests_,
            std::bind(&Request::ResetHandle, handle_, std::placeholders::_1)),
      watcherThread_(&CURLCore::watch, this) {}

CURLCore::~CURLCore() {
  killing_.store(true);
  watcherThread_.join();
  curl_multi_cleanup(handle_);
}

void CURLCore::watch() {
  log_debugs << "CURLCore: Watcher Thread started.";
  while (!killing_.load()) {
    start_pending();

    auto ret = check_handles();
    if (ret < 0) {
      log_errors << "Error waiting for curl requests.";
    }

    int active_requests = 0;
    auto err = curl_multi_perform(handle_, &active_requests);
    if (err != CURLM_OK) {
      log_errors << "Error with curl_multi_perform: "
                 << curl_multi_strerror(err);
    }

    handle_messages();
  }

  // We're dying so abort all pending requests.

  {
    std::lock_guard<std::mutex> lock(pending_mutex_);
    for (auto& iter : pending_) {
      auto pend = iter.lock();
      if (pend) {
        pend->abort();
      }
    }
  }
  for (auto& iter : active_) {
    auto active = iter.second.lock();
    if (active) {
      active->abort();
    }
  }
  log_debugs << "CURLCore: Watcher Thread finished.";
}

void CURLCore::start_pending() {
  handle_pool::Lease lease(pool_, false);
  if (!lease) {
    log_debugs << "CURLCore: No more leases.";
    return;
  }

  std::lock_guard<std::mutex> lock(pending_mutex_);
  while (pending_.size() && lease) {
    std::shared_ptr<Request> r;
    while (!r && pending_.size()) {
      r = pending_.front().lock();
      pending_.pop_front();
    }
    if ((bool)r) {
      CURL* h = lease->Handle;
      r->start(std::move(lease));
      active_[h] = r;
      curl_multi_add_handle(handle_, h);
    } else {
      log_debugs << "CURLCore: Request abandoned.";
    }
    lease = handle_pool::Lease(pool_, false);
  }
}

int CURLCore::check_handles() {
  auto timeout = find_timeout();
  fd_set fdread;
  fd_set fdwrite;
  fd_set fdexcep;
  int maxfd = -1;
  FD_ZERO(&fdread);
  FD_ZERO(&fdwrite);
  FD_ZERO(&fdexcep);
  curl_multi_fdset(handle_, &fdread, &fdwrite, &fdexcep, &maxfd);
  // TODO: Check return value.

  return select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
}

timeval CURLCore::find_timeout() {
  timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 100000;

  long curl_timeo = -1;

  curl_multi_timeout(handle_, &curl_timeo);
  if (curl_timeo >= 0) {
    timeout.tv_sec = curl_timeo / 1000;
    if (timeout.tv_sec > 1)
      timeout.tv_sec = 1;
    else
      timeout.tv_usec = (curl_timeo % 1000) * 1000;
  }
  return timeout;
}

void CURLCore::handle_messages() {
  CURLMsg* msg;
  int msgs_left;
  while ((msg = curl_multi_info_read(handle_, &msgs_left))) {
    if (msg->msg == CURLMSG_DONE) {
      auto req = active_[msg->easy_handle].lock();
      if ((bool)req) {
        req->finish();
      }
      active_.erase(msg->easy_handle);
    }
  }
}
}
}  // namespace om::network::