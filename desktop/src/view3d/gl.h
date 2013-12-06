#pragma once

#include <stdio.h>
#include "common/logging.h"
#include "utility/glInclude.h"

namespace om {
namespace gl {

/*
 * Standard Colors
 */
static const float OMGL_WHITE[3] = {1, 1, 1};
static const float OMGL_BLACK[3] = {0, 0, 0};
static const float OMGL_RED[3] = {1, 0, 0};
static const float OMGL_GREEN[3] = {0, 1, 0};
static const float OMGL_BLUE[3] = {0, 0, 1};

/*
 * Macro to check for silent errors that OpenGL builds up.
 * Can't be used inside of glBegin() / glEnd()
 * http://www.performanceopengl.com/s2002PerfOGL.ppt (in /docs folder)
 */
inline void CHECK_OPENGL_ERROR() {
  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR) {

    log_errors << "opengl failed with error " << gluErrorString(error);
  }
}

/*
 * gl* Macros
 */
inline void glTranslatefv(const float *v) { glTranslatef(v[0], v[1], v[2]); }

inline void glScalefv(const float *v) { glScalef(v[0], v[1], v[2]); }

inline void glColor3fva(const float *v, float a) {
  glColor4f(v[0], v[1], v[2], a);
}

/*
 * State Macro
 */
void pushGlState();
void popGlState();

/*
 *  Picking Utilities
 */
void startPicking(int x, int y, float perspective[4]);
int stopPicking();
void processHits(GLint hits, int **ppNamesRet, int *pNumNamesRet);
int unprojectPixel(int x, int y, GLdouble point[]);

/*
 *  Axis Drawing
 */
void glDrawAxis(int size = 3);
void glDrawPositiveAxis(int size = 3);

/*
 *  Shape Macro
 */
void omglWireCube(GLdouble size);
void omglSolidCube(GLdouble size);
}
}  // om::gl::
