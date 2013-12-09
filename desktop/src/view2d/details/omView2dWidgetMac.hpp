#pragma once

#include "viewGroup/omViewGroupState.h"

class OmView2dWidgetBase : public QWidget {
 protected:
  virtual void Initialize() {}
  virtual void Paint3D() = 0;
  virtual void PaintOther() = 0;
  virtual void Resize(int width, int height) = 0;

 public:
  OmView2dWidgetBase(QWidget* parent, OmViewGroupState* vgs)
      : QWidget(parent), vgs_(vgs) {
    Initialize();
    resetPbuffer(size());
  }

  inline void resetPbuffer(const QSize& size) {
    buffer_.reset(new QGLPixelBuffer(size, QGLFormat::defaultFormat(),
                                     vgs_->get3dContext()));
  }

  inline void resizeEvent(QResizeEvent* event) {
    Resize(event->size().width(), event->size().height());
    resetPbuffer(event->size());
  }

  inline virtual void paintEvent(QPaintEvent* event) {
    buffer_->makeCurrent();
    Paint3D();
    buffer_->doneCurrent();
    QImage view = buffer_->toImage();

    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), view);

    PaintOther();
  }

 private:
  std::unique_ptr<QGLPixelBuffer> buffer_;
  OmViewGroupState* vgs_;

};
