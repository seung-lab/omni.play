#pragma once
#include "precomp.h"
#include "system/omConnect.hpp"


class OmButton : public QPushButton {
Q_OBJECT

 public:
  /*
   * We should set left/right/toggle lambdas on construction!
   */
  OmButton(QWidget* mw, const QString& title, const QString& statusTip,
           const bool isCheckable)
      : QPushButton(mw), mParent(mw) {
    setText(title);
    setStatusTip(statusTip);
    setCheckable(isCheckable);

    om::connect(this, SIGNAL(clicked()), this,
        SLOT(callLeftClickAction()));
    om::connect(this, SIGNAL(rightClicked()), this,
        SLOT(callRightClickAction()));
    om::connect(this, SIGNAL(toggled(bool)), this,
        SLOT(callToggleAction(bool)));
  }

  void setKeyboardShortcut(const QString& shortcut) {
    setShortcut(QKeySequence(shortcut));
  }

  void setIconAndText(const QString& iconPath) {
    setIcon(QIcon(iconPath));
    if (!icon().isNull()) {
      setText("");
    }
  }

  void SetIcon() { setIcon(QIcon()); }

  void SetIcon(const QString& iconPath) { setIcon(QIcon(iconPath)); }

 private:
  QWidget* const mParent;
  virtual void onLeftClick() {}
  virtual void onRightClick() {}
  virtual void onToggle(bool isChecked) {}

  // Patch in signal for right clicking
  void mousePressEvent(QMouseEvent* event) {
    QPushButton::mousePressEvent(event);

    if (event->button() != Qt::RightButton) {
      return;
    }
    // A bit different from Qt src impl, we don't have have Q_D or Q_Q 
    // to call the signal, so we manually call the signal
    rightClicked();
  }

Q_SIGNALS:
  void rightClicked();

 protected Q_SLOTS:
  virtual void callLeftClickAction() { onLeftClick(); }
  virtual void callRightClickAction() { onRightClick(); }
  virtual void callToggleAction(bool isChecked) { onToggle(isChecked); }
};
