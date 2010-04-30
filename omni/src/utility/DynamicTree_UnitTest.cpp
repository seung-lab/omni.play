/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#include <iostream>
#include "zi/trees/DynamicTree.hpp"
#include "zi/zunit/zunit.h"

using namespace std;
using namespace Seung;

ZUNIT_TEST(BasicTests) {
  DynamicTree<int>* x[10];

  for (int i=0;i<10; i++)
    x[i] = DynamicTree<int>::makeTree(i+1);

  x[1]->join(x[2]);
  EXPECT_EQ(x[1]->findRoot(), x[2]);
  EXPECT_EQ(x[2]->findRoot(), x[2]);

  x[3]->join(x[2]);
  EXPECT_EQ(x[3]->findRoot(), x[2]);

  x[2]->join(x[6]);
  EXPECT_EQ(x[1]->findRoot(), x[6]);
  EXPECT_EQ(x[2]->findRoot(), x[6]);
  EXPECT_EQ(x[3]->findRoot(), x[6]);
  EXPECT_EQ(x[6]->findRoot(), x[6]);

  x[7]->join(x[8]);
  x[6]->join(x[7]);
  EXPECT_EQ(x[1]->findRoot(), x[8]);
  EXPECT_EQ(x[2]->findRoot(), x[8]);
  EXPECT_EQ(x[3]->findRoot(), x[8]);

  x[7]->cut();
  EXPECT_EQ(x[1]->findRoot(), x[7]);
  EXPECT_EQ(x[2]->findRoot(), x[7]);
  EXPECT_EQ(x[3]->findRoot(), x[7]);

  for (int i=0;i<10; i++)
    delete x[i];

}

ZUNIT_TEST(ChainTest) {
  DynamicTree<int>** aLot = new DynamicTree<int>*[4000000];
  for (int i=0;i<4000000; i++)
    aLot[i] = DynamicTree<int>::makeTree(i+1);

  for (int i=1;i<4000000; i++) {
    aLot[i-1]->join(aLot[i]);
    EXPECT_EQ(aLot[0]->findRoot(), aLot[i]);
  }

  for (int i=0;i<4000000; i++)
    delete aLot[i];

  delete aLot;
}

int main() {
  RUN_ALL_TESTS();
}

