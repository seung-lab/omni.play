#pragma once
#include "precomp.h"

#include "common/common.h"
#include "common/enums.hpp"
class CacheMonitorWidget : public QWidget {
  Q_OBJECT;

 public:
  CacheMonitorWidget(QWidget* parent);

Q_SIGNALS:
  void triggerCacheView();

 private:
  QGroupBox* showDisplay(const QString&, const om::common::CacheGroup);
};
