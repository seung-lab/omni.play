#pragma once
#include "precomp.h"

#include "common/logging.h"

namespace om {

inline static void connect(const QObject* sender, const char* signal,
                           const QObject* receiver, const char* method) {
  if (!QObject::connect(sender, signal, receiver, method,
                        Qt::QueuedConnection)) {
    log_infos << "failed making connection";
  }
}

}  // namespace om
