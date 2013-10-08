#pragma once

#include <QtGui>
#include <QWidget>

class Preferences : public QDialog {
  Q_OBJECT public : Preferences(QWidget *parent);
  void showProjectPreferences();
  void showLocalPreferences();

 private
Q_SLOTS:
  void closeDialog();

 private:
  QTabWidget *tabs;
  QPushButton *closeButton;

};
