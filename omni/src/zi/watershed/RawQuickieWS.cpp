#include "RawQuickieWS.h"

#include <boost/tuple/tuple.hpp>
#include <iostream>
#include <iomanip>
#include <vector>
#include <utility>
#include <algorithm>
#include <map>
#include <set>
#include <list>
#include <deque>
#include <limits>
#include <cmath>
#include <cstring>
#include <inttypes.h>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include "zi/omUtility.h"
#include "zi/trees/DisjointSets.hpp"

using namespace std;


void
rawQuickieWS(const float* connections,
             int64_t xDim, int64_t yDim, int64_t zDim,
             const float loThreshold,
             const float hiThreshold,
             int   noThreshold,
             const float absLowThreshold,
             uint32_t*  result,
             vector<pair<int64_t, float> > &graph,
             vector<pair<float, int64_t> > &dendQueue,
             vector<int> &sizes)
{

  cout << "Low Threshold: " << loThreshold << "\n"
       << "Hi  Threshold: " << hiThreshold << "\n"
       << "No  Threshold: " << noThreshold << "\n";


  int64_t xyDim      = xDim  * yDim;
  int64_t xyzDim     = xyDim * zDim;
  int64_t xyzDim2    = xyDim * zDim * 2;
  int64_t nHood[6]   = {-1, -xDim, -xyDim, 1, xDim, xyDim};

  int64_t totalNice  = 0;
  int64_t singleOnes = 0;
  int64_t nullOnes   = 0;

  //  memset((void*)result, 0, xyzDim * (int64_t)sizeof(int));


  for (int64_t i=0,d=0; d<3; ++d) {
    for (int64_t j=0,z=0; z<zDim; ++z)
      for (int64_t y=0; y<yDim; ++y)
        for (int64_t x=0; x<xDim; ++x,++i,++j) {
          if ((x == 0) && (d==0)) continue;
          if ((y == 0) && (d==1)) continue;
          if ((z == 0) && (d==2)) continue;
          if (connections[i] >= hiThreshold) {
            result[j]            |= (1 << d);
            result[j + nHood[d]] |= (8 << d);
          }
        }
  }


  for (int64_t j=0,z=0; z<zDim; ++z)
    for (int64_t y=0; y<yDim; ++y)
      for (int64_t x=0; x<xDim; ++x,++j) {

        int   d = -1;
        float top = 0.0, second = 0.0;

        if (x > 0) {
          if (connections[j] >= top) {
            d = 1;
            second = top;
            top = connections[j];
          }
        }

        if (y > 0) {
          if (connections[j+xyzDim] >= top) {
            d = 2;
            second = top;
            top = connections[j+xyzDim];
          }
        }

        if (z > 0) {
          if (connections[j+xyzDim2] >= top) {
            d = 3;
            second = top;
            top = connections[j+xyzDim2];
          }
        }


        if (x < xDim - 1) {
          if (connections[j+1] >= top) {
            d = 4;
            second = top;
            top = connections[j+1];
          }
        }


        if (y < yDim - 1) {
          if (connections[j+xyzDim+xDim] >= top) {
            d = 5;
            second = top;
            top = connections[j+xyzDim+xDim];
          }
        }


        if (z < zDim - 1) {
          if (connections[j+xyDim+xyzDim2] >= top) {
            d = 6;
            second = top;
            top = connections[j+xyDim+xyzDim2];
          }
        }

        if (d > 0) {
          if (top > absLowThreshold) {
            if (top > second) {
              --d;
              if (d < 3) {
                result[j]            |= (1 << d);
                result[j + nHood[d]] |= (8 << d);
              } else {
                result[j]            |= (1 << d);
                result[j + nHood[d]] |= (1 << (d - 3));
              }
              ++totalNice;
            } else {
              result[j] |= 0x10000000;
              ++singleOnes;
            }
          } else {
            ++nullOnes;
          }
        }
      }


  cout << "Abs Low Thresh:" << absLowThreshold << "\n";
  cout << "Total Nice   : " << totalNice << "\n";
  cout << "Total Singles: " << singleOnes << "\n";
  cout << "Total Null:    " << nullOnes << "\n";

  int64_t nonZero = 0;

  int            nextIdx = 1;
  deque<int64_t> queue;

  for (int64_t i=0; i<xyzDim; ++i) {

    if (i < 0 || i >= xyzDim) {
      cout << "ERROR\n";
      return;
    }

    if (!(result[i] & 0x80000000) && result[i]) {

      queue.push_back(i);
      sizes.push_back(0);

      while (queue.size() > 0) {

        int64_t cur = queue.front();
        queue.pop_front();

        for (int32_t z = 0; z < 6; ++z) {
          if (result[cur] & (1 << z)) {
            int64_t pot = cur + nHood[z];

            if (pot < 0 || pot >= xyzDim) {
              cout << "ERROR\n";
              return;
            }


            if (!(result[pot] & 0x80000000)) {
              result[pot] |= 0x80000000;
              queue.push_back(pot);
            }
          }
        }
        result[cur] = 0x80000000 | nextIdx;
        ++nonZero;
        ++sizes.back();
      }

      ++nextIdx;
    }

  }

  for (int64_t i=0; i<xyzDim; ++i) {
    result[i] = (result[i] & 0x80000000) ? 0x7FFFFFFF & result[i] : 0;
  }

  cout << "Non Zero : " << nonZero << "\n";

  boost::unordered_map<int64_t, float> uniques;

  int64_t totValidEdges = 0;
  for (int64_t i=0,d=0; d<3; ++d) {
    for (int64_t j=0,z=0; z<zDim; ++z)
      for (int64_t y=0; y<yDim; ++y)
        for (int64_t x=0; x<xDim; ++x,++i,++j) {
          if ((x == 0) && (d==0)) continue;
          if ((y == 0) && (d==1)) continue;
          if ((z == 0) && (d==2)) continue;
          if (connections[i] > loThreshold) {
            if (result[j] != result[j + nHood[d]]) {
              ++totValidEdges;
              int64_t L = std::min(result[j],  result[j + nHood[d]]);
              int64_t R = std::max(result[j],  result[j + nHood[d]]);
              L <<= 32;
              L |=  R;
              if (uniques[L] < connections[i]) {
                uniques[L] = connections[i];
              }
            } else {
              if (result[j] == 0) {
                ++totValidEdges;
              }
            }
          }
        }
  }

  /////////////////////////// Region graph done

  cout << "Valid  Edges : " << totValidEdges  << "\n";
  cout << "Unique Edges : " << uniques.size() << "\n";

  cout << "Total    : " << (nextIdx-1) << "\n";

  dendQueue.clear();

  FOR_EACH (it, uniques) {
    dendQueue.push_back(make_pair(it->second, it->first));
  }

  uniques.clear();

  sort(dendQueue.begin(), dendQueue.end());

  int64_t idxCount = nextIdx;
  zi::DisjointSets<int> trees(nextIdx);

  FOR_EACH_R (it, dendQueue) {
    if (it->second >> 32) {
      if (it->second < absLowThreshold) break;
      int64_t v1 = trees.findSet(it->second >> 32);
      int64_t v2 = trees.findSet(it->second & 0xFFFFFFFF);
      if ((v1 != v2) && (sizes[v1-1] < noThreshold || sizes[v2-1] < noThreshold)) {
        sizes[v2-1] += sizes[v1-1];
        sizes[v1-1]  = 0;
        std::swap(sizes[v2-1], sizes[trees.join(v1, v2) - 1]);
        --nextIdx;
      }
    }
  }

  // reusing sizes vector

  int64_t newIndex = 1;
  for (int64_t i=0; i<idxCount; ++i) {
    if (sizes[i] > 0) {
      sizes[i] = newIndex++;
    }
  }

  for (int64_t i=0; i<xyzDim; ++i) {
    result[i] = result[i] ? sizes[trees.findSet(result[i]) - 1] : 0;
  }

  graph.clear();

  FOR_EACH_R (it, dendQueue) {
    if (it->second >> 32) {
     int64_t v1 = trees.findSet(it->second >> 32);
     int64_t v2 = trees.findSet(it->second & 0xFFFFFFFF);
     if (v1 != v2) {
       v1 = sizes[v1-1];
       v2 = sizes[v2-1];
       v1 <<= 32;
       v1 |= v2;
       graph.push_back(make_pair(v1, it->first));
     }
    }
  }

  dendQueue.clear();
  trees.clear();

  list<pair<int, float> > edges[nextIdx];
  std::fill(sizes.begin(), sizes.end(), 0);

  FOR_EACH (it, graph) {
    if (it->first >> 32) {
     int64_t v1 = trees.findSet(it->first >> 32);
     int64_t v2 = trees.findSet(it->first & 0xFFFFFFFF);
     if (v1 != v2) {
       trees.join(v1, v2);
       //dendQueue.push_back(make_pair(it->second, it->first));
       edges[it->first >> 32].push_back(make_pair(it->first & 0xFFFFFFFF,
                                                  it->second));
       edges[it->first &0xFFFFFFFF].push_back(make_pair(it->first >> 32,
                                                        it->second));
     }
    }
  }

  deque<int32_t> bfs;
  for (int i=1;i<nextIdx;++i) {
    if (!sizes[i]) {
      bfs.push_back(i);
      sizes[i] = 1;
      while (!bfs.empty()) {
        FOR_EACH (it, edges[bfs.front()]) {
          if (!sizes[it->first]) {
            sizes[it->first] = 1;
            dendQueue.push_back(make_pair(it->second,
                                          (uint64_t)(it->first) << 32 |
                                          bfs.front()));
            bfs.push_back(it->first);
          }
        }
        bfs.pop_front();
      }
    }
  }

  sort(dendQueue.rbegin(), dendQueue.rend());

  cout << "Total Reduced : " << (nextIdx-1) << "\n";
  sizes.clear();

}

