/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef DYNAMIC_TREE_HPP
#define DYNAMIC_TREE_HPP

#include <assert.h>
#include <string>
#include <iostream>

namespace zi { namespace Trees {

#ifdef DYNAMIC_TREE_TEST
int zRefCnt = 0;
#endif

template <typename _T>
class DynamicTree;

template <typename _T>
class DynamicTree {
protected:
  typedef _T   value_t;
  typedef _T*  value_ptr;
  typedef _T&  value_ref;
  class Impl;
public:

  DynamicTree() : impl_(NULL) {}
  DynamicTree(const value_t &v);
  ~DynamicTree();

  void cut();
  void join(const DynamicTree &x);
  DynamicTree findRoot();
  bool isRoot();

  bool operator <   (const DynamicTree &x) const { return impl_ <  x.impl_; }
  bool operator ==  (const DynamicTree &x) const { return impl_ == x.impl_; }
  bool operator !=  (const DynamicTree &x) const { return impl_ != x.impl_; }

  DynamicTree &operator = (const DynamicTree &x);
  DynamicTree(const DynamicTree&);

protected:
  DynamicTree(Impl* impl);
  void incRef();
  void decRef();
  Impl* impl_;
  friend class Impl;
};

template <typename _T>
DynamicTree<_T>::DynamicTree(const value_t &v)
{
  impl_ = new Impl(v);
#ifdef DYNAMIC_TREE_TEST
  zRefCnt++;
#endif
}

template <typename _T>
DynamicTree<_T>::DynamicTree(const DynamicTree &t)
{
  impl_ = t.impl_;
  incRef();
}

template <typename _T>
DynamicTree<_T>::DynamicTree(Impl* impl)
{
  impl_ = impl;
  incRef();
}

template <typename _T>
DynamicTree<_T>::~DynamicTree()
{
  decRef();
}

template <typename _T>
void DynamicTree<_T>::decRef()
{
  if (impl_ != NULL) {
    if ((--impl_->refCount_) == 0) {
      impl_->cut();
      delete impl_;
#ifdef DYNAMIC_TREE_TEST
      zRefCnt--;
#endif
    }
  }
}

template <typename _T>
void DynamicTree<_T>::incRef()
{
  if (impl_ != NULL) {
    ++impl_->refCount_;
  }
}

template <typename _T>
DynamicTree<_T> &DynamicTree<_T>::operator = (const DynamicTree &x)
{
  if (x.impl_ == impl_)
    return *this;
  decRef();
  impl_ = x.impl_;
  incRef();
  return *this;
}

template <typename _T>
void DynamicTree<_T>::cut()
{
  if (impl_ != NULL)
    impl_->cut();
}

template <typename _T>
void DynamicTree<_T>::join(const DynamicTree &x)
{
  assert(x.impl_ != NULL);
  assert(impl_   != NULL);
  impl_->join(x.impl_);
}

template <typename _T>
DynamicTree<_T> DynamicTree<_T>::findRoot()
{
  if (impl_ != NULL)
    return DynamicTree<_T>(impl_->findRoot());
  else
    return DynamicTree<_T>(impl_);
}

template <typename _T>
bool DynamicTree<_T>::isRoot()
{
  if (impl_ != NULL)
    return impl_->isRoot();
  else
    return true;
}

template <typename _T>
class DynamicTree<_T>::Impl {
public:
  typedef typename DynamicTree::Impl    self_t;
  typedef typename DynamicTree::Impl*   self_ptr;
  typedef typename DynamicTree::value_t value_t;

  Impl(const value_t &x) :
    left_(NULL), right_(NULL), parent_(NULL), pathParent_(NULL),
    value_(x), refCount_(1) {}
    ~Impl() {}

  void rotateLeft() {
    assert(right_ != NULL);
    self_ptr x(right_), z(parent_);

    if (z != NULL) {
      if (z->left_ == this) {
        z->left_ = x;
      } else {
        assert(z->right_ == this);
        z->right_ = x;
      }
    } else {
      std::swap(x->pathParent_, pathParent_);
    }
    right_     = x->left_;
    x->left_   = this;
    x->parent_ = z;
    parent_    = x;

    if (right_ != NULL)
      right_->parent_ = this;
  }

  void rotateRight() {
    assert(left_ != NULL);

    self_ptr x(left_), z(parent_);
    if (z != NULL) {
      if (z->right_ == this) {
        z->right_ = x;
      } else {
        assert(z->left_ == this);
        z->left_ = x;
      }
    } else {
      std::swap(x->pathParent_, pathParent_);
    }
    left_      = x->right_;
    x->right_  = this;
    x->parent_ = z;
    parent_    = x;

    if (left_ != NULL)
      left_->parent_ = this;
  }

  void splay() {
    self_ptr g;
    while (parent_ != NULL) {
      g = parent_->parent_;
      if (this == parent_->left_) {
        if ((g != NULL) && (parent_ == g->left_)) g->rotateRight();
        parent_->rotateRight();
      } else {
        assert (this == parent_->right_);
        if ((g != NULL) && (parent_ == g->right_)) g->rotateLeft();
        parent_->rotateLeft();
      }
    }
  }

  void access() {

    splay();
    if (right_) {
      right_->pathParent_ = this;
      right_->parent_     = NULL;
      right_              = NULL;
    }

    self_ptr w, v = this;

    while (v->pathParent_ != NULL) {
      w = v->pathParent_;
      w->splay();
      if (w->right_ != NULL) {
        w->right_->pathParent_ = w;
        w->right_->parent_     = NULL;
      }
      w->right_      = v;
      v->parent_     = w;
      v->pathParent_ = NULL;
      v = w;
    }
    splay();
  }

  void cut() {
    access();
    if (left_ != NULL) {
      left_->parent_ = NULL;
      left_          = NULL;
    }
  }

  void join(self_ptr w) {
    access();
    assert(left_ == NULL);

    w->access();
    left_      = w;
    w->parent_ = this;
  }

  self_ptr findRoot() {
    access();
    self_ptr v = this;
    while (v->left_ != NULL) {
      v = v->left_;
    }
    v->splay();
    return v;
  }

  bool isRoot() {
    splay();
    return (left_ == NULL && pathParent_ == NULL);
  }

protected:
  friend class DynamicTree<_T>;
  self_ptr left_, right_, parent_, pathParent_;
  value_t  value_;
  int      refCount_;
};


template <typename T>
class DNMTREE {
public:

  virtual ~DNMTREE() {};

  void rotateLeft();
  void rotateRight();
  void splay();

  virtual DNMTREE* findMin();
  virtual DNMTREE* findMax();

  // link cut stuff
  void access();
  virtual DNMTREE* cut();
  void join(DNMTREE* w);
  virtual DNMTREE* findRoot();

  virtual void printTree(int d = 0);
  T& getKey() { return key_; }
  T  getKey() const { return key_; }
  void setKey(const T &key) { key_ = key; }

  static DNMTREE* makeTree(const T &key);

protected:
  DNMTREE() :
    left_(NULL), right_(NULL), parent_(NULL), pathParent_(NULL), key_("") {}
  DNMTREE(const T &key) :
    left_(NULL), right_(NULL), parent_(NULL), pathParent_(NULL), key_(key) {}
  DNMTREE(const T &key, DNMTREE *left,
              DNMTREE *right, DNMTREE* parent):
    left_(left), right_(right), parent_(parent), pathParent_(NULL), key_(key) {}

  DNMTREE *left_, *right_, *parent_, *pathParent_;
  T key_;
};

template<typename T>
DNMTREE<T>* DNMTREE<T>::makeTree(const T &key)
{
  return new DNMTREE(key);
}

template<typename T>
DNMTREE<T>* DNMTREE<T>::cut()
{
  access();
  if (left_ != NULL) {
    DNMTREE<T>* toRet = left_;
    left_->parent_ = NULL;
    left_          = NULL;
    return toRet->findMax();
  }
  return right_;
}

template<typename T>
void DNMTREE<T>::join(DNMTREE* w)
{
  access();
  assert(left_ == NULL);

  w->access();
  left_      = w;
  w->parent_ = this;
}

template<typename T>
DNMTREE<T>* DNMTREE<T>::findRoot()
{
  access();
  DNMTREE* v = this;
  while (v->left_ != NULL) {
    v = v->left_;
  }
  v->splay();
  return v;
}


template<typename T>
void DNMTREE<T>::access()
{
  splay();
  if (right_) {
    right_->pathParent_ = this;
    right_->parent_     = NULL;
    right_              = NULL;
  }
  DNMTREE *w, *v = this;

  while (v->pathParent_ != NULL) {
    w = v->pathParent_;
    w->splay();
    if (w->right_ != NULL) {
      w->right_->pathParent_ = w;
      w->right_->parent_     = NULL;
    }
    w->right_      = v;
    v->parent_     = w;
    v->pathParent_ = NULL;
    v = w;
  }
  splay();
}


template<typename T>
void DNMTREE<T>::rotateLeft()
{
  assert(right_ != NULL);

  DNMTREE *x(right_), *z(parent_);
  if (z != NULL) {
    if (z->left_ == this) {
      z->left_ = x;
    } else {
      assert(z->right_ == this);
      z->right_ = x;
    }
  } else {
    std::swap(x->pathParent_, pathParent_);
  }
  right_     = x->left_;
  x->left_   = this;
  x->parent_ = z;
  parent_    = x;

  if (right_ != NULL)
    right_->parent_ = this;
}

template<typename T>
void DNMTREE<T>::rotateRight()
{
  assert(left_ != NULL);

  DNMTREE *x(left_), *z(parent_);
  if (z != NULL) {
    if (z->right_ == this) {
      z->right_ = x;
    } else {
      assert(z->left_ == this);
      z->left_ = x;
    }
  } else {
    std::swap(x->pathParent_, pathParent_);
  }
  left_      = x->right_;
  x->right_  = this;
  x->parent_ = z;
  parent_    = x;

  if (left_ != NULL)
    left_->parent_ = this;
}

template<typename T>
void DNMTREE<T>::splay()
{
  DNMTREE *g;
  while (parent_ != NULL) {
    g = parent_->parent_;
    if (this == parent_->left_) {
      if ((g != NULL) && (parent_ == g->left_)) g->rotateRight();
      parent_->rotateRight();
    } else {
      assert (this == parent_->right_);
      if ((g != NULL) && (parent_ == g->right_)) g->rotateLeft();
      parent_->rotateLeft();
    }
  }
}

template<typename T>
void DNMTREE<T>::printTree(int d)
{
  if (left_) left_->printTree(d+1);
  //std::cout << std::string(d, ' ') << key_ << " ("
  //          << (pathParent_?pathParent_->key_ : T()) << ")" << std::endl;
  if (right_) right_->printTree(d+1);
}

template<typename T>
DNMTREE<T>* DNMTREE<T>::findMin()
{
  DNMTREE* x = this;
  while (x->left_ != NULL)
    x = x->left_;
  x->splay();
  return x;
}

template<typename T>
DNMTREE<T>* DNMTREE<T>::findMax()
{
  DNMTREE* x = this;
  while (x->right_ != NULL)
    x = x->right_;
  x->splay();
  return x;
}



} }

#endif
