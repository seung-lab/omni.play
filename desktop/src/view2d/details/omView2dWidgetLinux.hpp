#pragma once

class OmView2dWidgetBase : public QGLWidget
{
protected:
    virtual void Initialize() = 0;
    virtual void Paint3D() = 0;
    virtual void PaintOther() = 0;
    virtual void Resize(int width, int height) = 0;

public:
	OmView2dWidgetBase(QWidget* parent, OmViewGroupState*)
		: QGLWidget(parent)
	{}

	inline void initializeGL() {
		Initialize();
	}

    inline void resizeGL(int width, int height) {
    	Resize(width, height);
    }

    inline virtual void paintGL(){
    	Paint3D();
    	PaintOther();
    }
};
