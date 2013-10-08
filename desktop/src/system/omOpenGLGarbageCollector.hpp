#pragma once

/**
 * Collect OpengL texture IDs (View2d) and Display List IDs (View3d) of
 * tiles/meshes
 * freed from threaded caches; GL resources are freed when View2d/View3d decide
 * to
 * perform garbage collection
 *
 * Since View2d/3d have their own OpenGL contexts, the deletion of the View2d/3d
 * widget
 *  will free all resources it has allocated. Thus, we don't have to perform a
 * cleanup
 *  operation when the widget and/or project is destroyed.
 *
 * Must be called from main GUI thread.
 *
 * Michael Purcaro (purcaro@gmail.com) April 2011
 * Matt Wimer mwimer@mit.edu 10/21/09
 **/

#include "common/omCommon.h"
#include "common/omContainer.hpp"
#include "common/omGl.h"
#include "utility/omTimer.hpp"

#include "zi/omMutex.h"
#include "zi/omUtility.h"

class QGLContext;

class OmOpenGLGarbageCollector
    : private om::singletonBase<OmOpenGLGarbageCollector> {
 public:
  static void Clear() {
    om::container::clear(instance().textureIDs_);
    om::container::clear(instance().displayListIDs_);
  }

  static void AddTextureID(QGLContext const* context, const GLuint textureID) {
    zi::guard g(instance().textureListLock_);
    instance().textureIDs_[context].push_back(textureID);
  }

  static void CleanTextureIDs(QGLContext const* context) {
    glIntVec_t vec;

    {
      zi::guard g(instance().textureListLock_);

      if (instance().textureIDs_.count(context)) {
        instance().textureIDs_[context].swap(vec);
      }
    }

    if (vec.empty()) {
      return;  // required for Mac OS X
    }

    glDeleteTextures(vec.size(), &vec[0]);
  }

  static void AddDisplayListID(QGLContext const* context,
                               const GLuint displayListID) {
    zi::guard g(instance().displayListLock_);
    instance().displayListIDs_[context].push_back(displayListID);
  }

  static void CleanDisplayLists(QGLContext const* context) {
    glIntVec_t vec;

    {
      zi::guard g(instance().displayListLock_);

      if (instance().displayListIDs_.count(context)) {
        instance().displayListIDs_[context].swap(vec);
      }
    }

    FOR_EACH(iter, vec) { glDeleteLists((*iter), 1); }
  }

 private:
  OmOpenGLGarbageCollector() {}

  ~OmOpenGLGarbageCollector() {}

  typedef std::vector<GLuint> glIntVec_t;

  zi::spinlock textureListLock_;
  std::map<QGLContext const*, glIntVec_t> textureIDs_;

  zi::spinlock displayListLock_;
  std::map<QGLContext const*, glIntVec_t> displayListIDs_;

  friend class zi::singleton<OmOpenGLGarbageCollector>;
};
