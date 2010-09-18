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

using namespace std;
using boost::unordered_map;

#include "MemMap.hpp"

DECLARE_MEX(QuickieWatershed);

#ifndef ARRAYSIZE
#  define ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))
#endif

void computeRegionGraphEdges(const vector<graph_t> &graph,
			     mxArray *plhs[])
{
  DECLARE_MEX_RET_ARRAY(retGraph, plhs[1], int, graph.size(), 2);
  int idx = 0;
  FOR_EACH (it, graph) retGraphData[idx++] = (it->first >> 32);
  FOR_EACH (it, graph) retGraphData[idx++] = (it->first & 0xFFFFFFFF);
}

void computeRegionGraphThresholds(const vector<graph_t> &graph,
				  mxArray *plhs[])
{
  DECLARE_MEX_RET_ARRAY(retGraphVals, plhs[2], float, graph.size(), 1);
  int idx = 0;
  FOR_EACH (it, graph) retGraphValsData[idx++] = (float)it->second;
}

void computeMSTedges(const vector<dend_t> dend,
		     mxArray *plhs[])
{
  DECLARE_MEX_RET_ARRAY(retDend, plhs[3], int, dend.size(), 2);
  int idx = 0;
  FOR_EACH (it, dend) retDendData[idx++] = (it->second >> 32);
  FOR_EACH (it, dend) retDendData[idx++] = (it->second & 0xFFFFFFFF);
}

void computeMSTthresholds(const vector<dend_t> dend,
			  mxArray *plhs[])
{
  DECLARE_MEX_RET_ARRAY(retDendVals, plhs[4], float, dend.size(), 1);
  int idx = 0;
  FOR_EACH (it, dend) retDendValsData[idx++] = (float)it->first;
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  MEXINIT();

  const size_t xDim = getMexScalar<size_t>(prhs[0]);
  const size_t yDim = getMexScalar<size_t>(prhs[1]);
  const size_t zDim = getMexScalar<size_t>(prhs[2]);
  const float loThreshold     = getMexScalar<float>(prhs[3]);
  const float hiThreshold     = getMexScalar<float>(prhs[4]);
  const int   sizeThreshold     = getMexScalar<int>(prhs[5]);
  const float absLowThreshold = getMexScalar<float>(prhs[6]);

  cout << "x,y,z == " << xDim << "," << yDim << "," << zDim << "\n";

  const size_t numVoxels = xDim*yDim*zDim;
  const size_t numBytesIn = numVoxels*sizeof(float)*3;
  const size_t numbytesOut = numVoxels*sizeof(int);

  /*
  MemMappedFileRead<float> in("/scratch/purcaro/connE1088full.raw",
			      numBytesIn);
  MemMappedFileWrite<int> out("/scratch/purcaro/connE1088full.output.raw",
			      numbytesOut);
  */
  const std::string dirName = "/home/omni/data/alum_sirini_sept-10-2010/";
  //  const std::string inFile = dirName + "connE1088full.raw";
  const std::string inFile = dirName + "test.raw";
  const std::string outFile = inFile + ".out";
  MemMappedFileRead<float> in(inFile, numBytesIn);
  MemMappedFileWrite<int> out(outFile, numbytesOut);

  MEXPRINTF("Calling algorithm...");
  MEXFLUSH();

  RawQuickieWS rqws(xDim,
		    yDim,
		    zDim,
		    loThreshold,
		    hiThreshold,
		    sizeThreshold,
		    absLowThreshold);

  rqws.Run(in.GetMapPtr(), out.GetMapPtr());

  MEXPRINTF("DONE");
  MEXFLUSH();

  if (nlhs < 1) MEX_RETURN;

  computeMSTedges(rqws.GetDend(), plhs);
  if (nlhs < 2) MEX_RETURN;

  computeMSTthresholds(rqws.GetDend(), plhs);
  if (nlhs < 3) MEX_RETURN;

  computeRegionGraphEdges(rqws.GetGraph(), plhs);
  if (nlhs < 4) MEX_RETURN;

  computeRegionGraphThresholds(rqws.GetGraph(), plhs);
  MEX_RETURN;
}
