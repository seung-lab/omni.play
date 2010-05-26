/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef DYNAMIC_TREE_H
#define DYNAMIC_TREE_H

#include <assert.h>
#include <string>
#include <iostream>

template <typename T>
class DynamicTree {
public:

  virtual ~DynamicTree() {};

  virtual DynamicTree* rotateLeft();
  virtual DynamicTree* rotateRight();
  virtual DynamicTree* splay();

  virtual DynamicTree* findMin();
  virtual DynamicTree* findMax();

  // link cut stuff
  virtual DynamicTree* access();
  virtual DynamicTree* cut();
  virtual DynamicTree* join(DynamicTree* w);
  virtual DynamicTree* findRoot();

  virtual void printTree(int d = 0);
  T& getKey() { return key_; }
  T getKey() const { return key_; }
  void setKey(const T &key) { key_ = key; }
  virtual DynamicTree* getParent() { return parent_; }

  static DynamicTree* makeTree(const T &key);

protected:
  DynamicTree() : left_(NULL), right_(NULL), parent_(NULL), pathParent_(NULL), key_(0) {}
  DynamicTree(const T &key) :
    left_(NULL), right_(NULL), parent_(NULL), pathParent_(NULL), key_(key) {}
  DynamicTree(const T &key, DynamicTree *left, DynamicTree *right, DynamicTree* parent):
    left_(left), right_(right), parent_(parent), pathParent_(NULL), key_(key) {}

  DynamicTree *left_, *right_, *parent_, *pathParent_;
  T key_;
};

#endif
