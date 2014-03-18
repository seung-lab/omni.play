#pragma once
#include "precomp.h"

#include "gui/cacheMonitorWidget.h"

class MainWindow;

class CacheMonitorDialog : public QDialog {
  Q_OBJECT;

 public:
  CacheMonitorDialog(QWidget* parent);
  ~CacheMonitorDialog();

 private
Q_SLOTS:
  void Refresh();
  void done(int);

 private:
  QVBoxLayout* layout;
  QTimer* mTimer;
  CacheMonitorWidget* mCacheMonitorWidget;
  bool mDying;
};
