#pragma once
#include "precomp.h"
#include "system/omConnect.hpp"
#include "functional"

class OmButton : public QPushButton {
Q_OBJECT

 public:
  /*
   * We should set left/right/toggle lambdas on construction!
   */
  OmButton(QWidget* mw, const QString& title, const QString& statusTip,
           const bool isCheckable,
           std::function<void(bool)> onLeftClickLambda = {},
           std::function<void(bool)> onRightClickLambda = {},
           std::function<void(bool)> onToggleLambda = {})
      : QPushButton(mw), mParent(mw),
        onLeftClickLambda_(onLeftClickLambda),
        onRightClickLambda_(onRightClickLambda),
        onToggleLambda_(onToggleLambda) {
    setText(title);
    setStatusTip(statusTip);
    setCheckable(isCheckable);

    om::connect(this, SIGNAL(clicked(bool)), this,
        SLOT(callLeftClick(bool)));
    om::connect(this, SIGNAL(rightClicked(bool)), this,
        SLOT(callRightClick(bool)));
    om::connect(this, SIGNAL(toggled(bool)), this,
        SLOT(callToggle(bool)));
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

  const std::function<void(bool)> onLeftClickLambda_;
  const std::function<void(bool)> onRightClickLambda_;
  const std::function<void(bool)> onToggleLambda_;

  // Patch in signal for right clicking
  void mousePressEvent(QMouseEvent* event) {
    QPushButton::mousePressEvent(event);

    if (event->button() != Qt::RightButton) {
      return;
    }
    // A bit different from Qt src impl, we don't have have Q_D or Q_Q 
    // to call the signal, so we manually call the signal
    rightClicked(isChecked());
  }


 protected Q_SLOTS:
  void callLeftClick(bool isChecked) {
    if(onLeftClickLambda_) {
      onLeftClickLambda_(isChecked);
    } else {
      onLeftClick();
    }
  }

  void callRightClick(bool isChecked) {
    if(onRightClickLambda_) {
      onRightClickLambda_(isChecked);
    } else {
      onRightClick();
    }
  }

  void callToggle(bool isChecked) {
    if(onToggleLambda_) {
      onToggleLambda_(isChecked);
    } else {
      onToggle(isChecked);
    }
  }

Q_SIGNALS:
  void rightClicked(bool isChecked);
};
