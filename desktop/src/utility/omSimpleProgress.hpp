#pragma once
#include "precomp.h"

#include "utility/omLockedPODs.hpp"
#include "threads/taskManager.hpp"
#include "utility/omTimer.hpp"

class OmSimpleProgress {
 private:
  const int max_;
  const std::string title_;

  LockedInt32 cur_;
  size_t lineLen_;

  om::thread::ThreadPool threadPool_;
  OmTimer timer_;

 public:
  OmSimpleProgress(const int max, const std::string& title)
      : max_(max), title_(title), lineLen_(0) {
    cur_.set(0);

    threadPool_.start(1);
    log_infos << title;
  }

  ~OmSimpleProgress() {
    log_infos << "done with " << title_;
    threadPool_.join();
  }

  void Join() { threadPool_.join(); }

  void DidOne(const std::string prefix = "") {
    threadPool_.push_back(
        zi::run_fn(zi::bind(&OmSimpleProgress::didOne, this, prefix)));
  }

 private:
  void didOne(const std::string prefix) {
    cur_.inc();

    coutLine(prefix);

    if (max_ == cur_.get()) {
      printDone();
    }
  }

  void printDone() { timer_.Print("done with " + title_); }

  void coutLine(const std::string& prefix) {
    log_infos << prefix << ": " << cur_.get() << " of " << max_;
  }
};
