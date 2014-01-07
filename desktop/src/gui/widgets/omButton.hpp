#pragma once

#include <QtGui>

template <class T>
class OmButton : public QPushButton {
 public:
  OmButton(T* mw, const QString& title, const QString& statusTip,
           const bool isCheckable)
      : QPushButton(mw), mParent(mw) {
    setText(title);
    setStatusTip(statusTip);
    setCheckable(isCheckable);
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

 protected:
  T* const mParent;
  virtual void doAction() = 0;
  virtual void doRightClick() {}

  void mousePressEvent(QMouseEvent* event) {
    QPushButton::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
      doAction();
    } else if (event->button() == Qt::RightButton) {
      doRightClick();
    }
  }
};
