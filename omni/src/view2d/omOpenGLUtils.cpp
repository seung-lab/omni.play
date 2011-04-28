#include "view2d/omOpenGLUtils.h"
#include "common/omGl.h"

void om::opengl_::SetupGLblendColor(const bool haveAlphaGoBlack, const float alpha,
                                    const bool shouldBrightenAlpha)
{
    glEnable(GL_BLEND);  // enable blending for transparency

    if(haveAlphaGoBlack){
        glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
        glBlendColor(1.f, 1.f, 1.f, (1.f - alpha));

    } else {
        glBlendFuncSeparate(GL_SRC_COLOR, GL_CONSTANT_COLOR,
                            GL_ONE_MINUS_CONSTANT_ALPHA, GL_CONSTANT_ALPHA);

        float factor = 0.4;
        if(shouldBrightenAlpha){
            factor = 0.7;
        }
        const float val = alpha * factor;
        glBlendColor(val, val, val, val);
    }
}
