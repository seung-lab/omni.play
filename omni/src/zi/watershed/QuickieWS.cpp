/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#include "zi/matlab/zmex.hpp"
#include <zi/utility>
#include "RawQuickieWS.h"

#include <iostream>
#include <cstdlib>
#include <vector>
#include <stack>
#include <list>
#include <boost/unordered_map.hpp>

using namespace std;
using boost::unordered_map;

DECLARE_MEX(QuickieWatershed);

#ifndef ARRAYSIZE
#  define ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))
#endif

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

  MEXINIT();

  DECLARE_MEX_ARRAY_VARS(conn, prhs[0], float);
  MEX_ASSERT_NDIM(conn, 4);

  float loThreshold     = getMexScalar<float>(prhs[1]);
  float hiThreshold     = getMexScalar<float>(prhs[2]);
  int   noThreshold     = getMexScalar<int>(prhs[3]);
  float absLowThreshold = getMexScalar<float>(prhs[4]);

  vector<int> sizes;

  DECLARE_MEX_RET_ARRAY(labels, plhs[0], int,
                        connDims[0], connDims[1], connDims[2]);

  MEXPRINTF("Calling CC Algorithm");
  MEXFLUSH();

  vector<pair<int64_t, float> > graph;
  vector<pair<float, int64_t> > dend;

  rawQuickieWS(connData,
               connDims[0], connDims[1], connDims[2],
               loThreshold,
               hiThreshold,
               noThreshold,
               absLowThreshold,
               labelsData,
               graph,
               dend,
               sizes);
  MEXPRINTF("Calling CC Algorithm DONE");
  MEXFLUSH();

  if (nlhs < 2) MEX_RETURN;

  DECLARE_MEX_RET_ARRAY(retGraph, plhs[1], int, graph.size(), 2);

  int idx = 0;
  FOR_EACH (it, graph) retGraphData[idx++] = (it->first >> 32);
  FOR_EACH (it, graph) retGraphData[idx++] = (it->first & 0xFFFFFFFF);

  if (nlhs < 3) MEX_RETURN;

  DECLARE_MEX_RET_ARRAY(retGraphVals, plhs[2], float, graph.size(), 1);

  idx = 0;
  FOR_EACH (it, graph) retGraphValsData[idx++] = (float)it->second;

  if (nlhs < 4) MEX_RETURN;

  DECLARE_MEX_RET_ARRAY(retDend, plhs[3], int, dend.size(), 2);

  idx = 0;
  FOR_EACH (it, dend) retDendData[idx++] = (it->second >> 32);
  FOR_EACH (it, dend) retDendData[idx++] = (it->second & 0xFFFFFFFF);

  if (nlhs < 5) MEX_RETURN;

  DECLARE_MEX_RET_ARRAY(retDendVals, plhs[4], float, dend.size(), 1);

  idx = 0;
  FOR_EACH (it, dend) retDendValsData[idx++] = (float)it->first;

  MEX_RETURN;

}
