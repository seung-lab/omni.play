/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef FHEAP_HPP_
#define FHEAP_HPP_

#include <assert.h>
#include <string>
#include <iostream>
#include <algorithm>

namespace zi { namespace Trees {

template <typename T>
class FHeapable;

template <typename T>
class FHeap {
public:
  typedef T            value_t;
  typedef FHeapable<T> heapable_t;
  FHeap() : rootList_(0), min_(0), size_(0) {}
  ~FHeap() { clear(); }
  void clear();
  void push(const T& v);
  template <typename C>
  void push(const T& v, C &hp);
  template <typename C>
  void erase(C &hp);
  template <typename C>
  C* topNode() { return reinterpret_cast<C*>(min_->heapable_); }
  T top() { return min_->value_; }
  void pop();
  int size() { return size_; }
protected:
  friend class FHeapable<T>;
  struct Node;
  void print(Node *x, int depth);
  void heapLink(Node *y, Node* x);
  void cut(Node *x, Node* y);
  void cascadingCut(Node *y);
  void addToRootList(Node *x);
  void removeFromRootList(Node *x);
  void consolidate();
  Node* rootList_;
  Node* min_     ;
  int   size_    ;
};

template <typename T>
void FHeap<T>::push(const T& v) {
  Node *node = new Node(v);
  addToRootList(node);
  ++size_;
}

template <typename T> template <typename C>
void FHeap<T>::push(const T& v, C &hp) {
  Node *node = new Node(v);
  hp.heap_ = this;
  hp.node_ = node;
  node->heapable_ = &hp;
  addToRootList(node);
  ++size_;
}

template <typename T> template <typename C>
void FHeap<T>::erase(C &hp) {
  assert(hp.heap_ == this);
  assert(hp.node_->heapable_ == &hp);

  Node *node = hp.node_;

  if (node->parent_ != NULL)
    cut(node, NULL);

  min_ = node;
  pop();
}

template <typename T>
void FHeap<T>::heapLink(FHeap::Node *y, FHeap::Node *x) {
  removeFromRootList(y);
  x->addChild(y);
  y->mark_ = false;
}

template <typename T>
void FHeap<T>::cut(FHeap::Node *x, FHeap::Node *) {
  x->detach();
  x->mark_   = 0;
  addToRootList(x);
}

template <typename T>
void FHeap<T>::cascadingCut(FHeap::Node *y) {
  Node* z = y->parent_;
  while (z != 0) {
    if (!y->mark_) {
      y->mark_ = true;
      break;
    } else {
      cut(y, z);
      y = z;
    }
  }
}

template <typename T>
void FHeap<T>::pop() {
  min_->heapable_ = NULL;
  Node *x = min_->child_;
  if (x != NULL) {
    do {
      x->parent_ = 0;
      x = x->right_;
    } while (x != min_->child_);

    x->left_->right_ = rootList_;
    rootList_->left_->right_ = x;
    std::swap(rootList_->left_, x->left_);
  }

  removeFromRootList(min_);
  if (min_->right_ == min_) {
    delete min_;
    min_ = NULL;
  } else {
    Node* toDel = min_;
    min_ = min_->right_;
    delete toDel;
    consolidate();
  }

  --size_;
}

template <typename T>
void FHeap<T>::addToRootList(FHeap::Node *x) {
  if (min_ == NULL || min_->value_ > x->value_)
    min_ = x;
  if (rootList_) {
    x->right_ = rootList_;
    x->left_  = rootList_->left_;
    rootList_->left_->right_ = x;
    rootList_->left_ = x;
  } else {
    rootList_ = x;
  }
}

template <typename T>
void FHeap<T>::removeFromRootList(FHeap::Node *x) {
  x->right_->left_ = x->left_;
  x->left_->right_ = x->right_;
  if (rootList_ == x) {
    rootList_ = (x == x->left_) ? 0 : x->left_;
  }
}

template <typename T>
void FHeap<T>::clear() {
  if (rootList_ == NULL)
    return;

  Node *z = rootList_, *n;
  do {
    n = z->right_;
    z->clearChildren();
    delete z;
    z = n;
  } while (z != rootList_);

  min_ = rootList_ = NULL;
}

template <typename T>
void FHeap<T>::consolidate() {
  Node* A[45] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  Node *z = min_, *n;
  int maxd = 0;
  do {
    n = z->right_;
    z->left_ = z->right_ = z;
    int d = z->degree_;
    while (A[d] != NULL) {
      if (A[d]->value_ < z->value_) {
        heapLink(z, A[d]);
        z = A[d];
      } else {
        heapLink(A[d], z);
      }
      A[d] = NULL;
      ++d;
    }
    A[d] = z;
    if (d > maxd) maxd = d;
    z = n;
  } while (z != min_);
  rootList_ = NULL;
  for (int d=0;d<=maxd;d++)
    if (A[d] != 0)
      addToRootList(A[d]);
}


template <typename T>
struct FHeap<T>::Node {
  typedef typename FHeap::Node        self_t;
  typedef typename FHeap::Node*       self_ptr;
  typedef typename FHeap::value_t     value_t;
  typedef typename FHeap::heapable_t  heapable_t;
  typedef typename FHeap::heapable_t* heapable_ptr;
  value_t  value_;
  self_ptr left_, right_, parent_, child_;
  int      degree_;
  bool     mark_;
  heapable_ptr heapable_;
  Node(const value_t &v) :
    value_(v), left_(this), right_(this),
    parent_(0), child_(0), degree_(0), mark_(0), heapable_(0) {}
  void addChild(Node *n) {
    ++degree_;
    if (child_) {
      n->right_ = child_;
      n->left_  = child_->left_;
      child_->left_->right_  = n;
      child_->left_          = n;
    } else {
      child_ = n;
      n->left_ = n->right_ = n;
    }
    n->parent_ = this;
  }
  void detach() {
    --parent_->degree_;
    parent_->child_ = (left_ != this) ? left_ : NULL;
    left_->right_ = right_;
    right_->left_ = left_;
    parent_ = NULL;
  }
  void clearChildren() {
    if (child_ == NULL)
      return;
    Node *z = child_, *n;
    do {
      n = z->right_;
      z->clearChildren();
      delete z;
      z = n;
    } while (z != child_);
  }
};

template <typename T>
class FHeapable {
public:
  FHeapable() {};
  virtual ~FHeapable() {};
protected:
  typedef typename FHeap<T>::Node node_t;
  friend class FHeap<T>;
  FHeap<T>  *heap_;
  node_t    *node_;
};


} }

#endif
