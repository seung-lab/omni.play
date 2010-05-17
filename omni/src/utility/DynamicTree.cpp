#include "DynamicTree.h"

template<typename T>
DynamicTree<T>* DynamicTree<T>::makeTree(const T &key)
{
  return new DynamicTree(key);
}

template<typename T>
DynamicTree<T>* DynamicTree<T>::cut()
{
  access();
  if (left_ != NULL) {
    left_->parent_ = NULL;
    left_          = NULL;
  }
  return this;
}

template<typename T>
DynamicTree<T>* DynamicTree<T>::join(DynamicTree* w)
{
  access();
  assert(left_ == NULL);

  w->access();
  left_      = w;
  w->parent_ = this;

  return this;
}

template<typename T>
DynamicTree<T>* DynamicTree<T>::findRoot()
{
  access();
  DynamicTree* v = this;
  while (v->left_ != NULL) {
    v = v->left_;
  }
  return v->splay();
}

template<typename T>
DynamicTree<T>* DynamicTree<T>::access()
{
  splay();
  if (right_) {
    right_->pathParent_ = this;
    right_->parent_     = NULL;
    right_              = NULL;
  }
  DynamicTree *w, *v = this;

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
  return splay();
}

template<typename T>
DynamicTree<T>* DynamicTree<T>::rotateLeft()
{
  assert(right_ != NULL);

  DynamicTree *x(right_), *z(parent_);
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

  return this;
}

template<typename T>
DynamicTree<T>* DynamicTree<T>::rotateRight()
{
  assert(left_ != NULL);

  DynamicTree *x(left_), *z(parent_);
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

  return this;
}

template<typename T>
DynamicTree<T>* DynamicTree<T>::splay()
{
  DynamicTree *g;
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
  return this;
}

template<typename T>
void DynamicTree<T>::printTree(int d)
{
  if (left_) left_->printTree(d+1);
  
  std::cout << std::string(d, ' ') << key_ << " ("
	    << (pathParent_?pathParent_->key_ : T()) << ")" << std::endl;

  if (right_) right_->printTree(d+1);
}

template<typename T>
DynamicTree<T>* DynamicTree<T>::findMin()
{
  DynamicTree* x = this;
  while (x->left_ != NULL)
    x = x->left_;
  return x->splay();
}

template<typename T>
DynamicTree<T>* DynamicTree<T>::findMax()
{
  DynamicTree* x = this;
  while (x->right_ != NULL)
    x = x->right_;
  return x->splay();
}
