#pragma once
#include "precomp.h"

namespace om {
namespace common {

class IProgress {
 public:
  virtual ~IProgress() {}
  virtual uint32_t GetTotal() const = 0;
  virtual void SetTotal(const uint32_t total) = 0;
  virtual uint32_t GetDone() const = 0;
  virtual void SetDone(const uint32_t numDid) = 0;
};
}
}