/* Marker-based watershed segmentation
 * developed and maintained by Srinivas C. Turaga <sturaga@mit.edu>
 * do not distribute without permission.
 */


#include "mex.h"
#include <iostream>
#include <cstdlib>
#include <boost/pending/disjoint_sets.hpp>
#include <vector>
#include <map>
#include <queue>
using namespace std;

// zero-based sub2ind
mwSize sub2ind(
		const mwSize * sub,
		const mwSize num_dims,
		const mwSize * dims
		)
{
	mwSize ind = 0;
	mwSize prod = 1;
	for (mwSize d=0; d<num_dims; d++) {
		ind += sub[d] * prod;
		prod *= dims[d];
	}
	return ind;
}

// zero-based ind2sub
void ind2sub(
		mwSize ind,
		const mwSize num_dims,
		const mwSize * dims,
		mwSize * sub
		)
{
	for (mwSize d=0; d<num_dims; d++) {
		sub[d] = (ind % dims[d]);
		ind /= dims[d];
	}
	return;
}



class mycomp{
    const float * conn_data;
    public:
        mycomp(const float * conn_data_param){
            conn_data = conn_data_param;
        }
        bool operator() (const mwSize& ind1, const mwSize& ind2) const {
            return conn_data[ind1]<conn_data[ind2];
        }
};
    
//MAXIMUM spanning tree
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]){
    const mxArray * conn = prhs[0];
	const mwSize conn_num_dims = mxGetNumberOfDimensions(conn);
	const mwSize * conn_dims = mxGetDimensions(conn);
	const mwSize conn_num_elements = mxGetNumberOfElements(conn);
	const float * conn_data =(const float *)mxGetData(conn);
	const mxArray * nhood = prhs[1];
	const mwSize nhood_num_dims = mxGetNumberOfDimensions(nhood);
	const mwSize * nhood_dims = mxGetDimensions(nhood);
	const double * nhood_data = (const double *)mxGetData(nhood);
    const mxArray * marker = prhs[2];
	const mwSize marker_num_dims = mxGetNumberOfDimensions(marker);
	const mwSize * marker_dims = mxGetDimensions(marker);
	const mwSize num_vertices = mxGetNumberOfElements(marker);
	const uint32_t * marker_data =(const uint32_t *)mxGetData(marker);
    const mxArray * grow_mask = prhs[3];
	const mwSize grow_mask_num_dims = mxGetNumberOfDimensions(grow_mask);
	const mwSize * grow_mask_dims = mxGetDimensions(grow_mask);
	const mxLogical * grow_mask_data =(const mxLogical *)mxGetLogicals(grow_mask);
	const double low_threshold = (const double) mxGetScalar(prhs[4]);
	
    if (!mxIsSingle(conn)){
        mexErrMsgTxt("Conn array must be floats (singles)");
    }
    if (nhood_num_dims != 2) {
		mexErrMsgTxt("wrong size for nhood");
	}
	if ((nhood_dims[1] != (conn_num_dims-1))
		|| (nhood_dims[0] != conn_dims[conn_num_dims-1])){
		mexErrMsgTxt("nhood and conn dimensions don't match");
	}

	// create disjoint sets
	//   mwSize num_vertices = marker_num_elements;	//conn_dims[0]*conn_dims[1]*conn_dims[2];
    std::vector<mwSize> rank(num_vertices);
    std::vector<mwSize> parent(num_vertices);
    boost::disjoint_sets<mwSize*, mwSize*> dsets(&rank[0],&parent[0]);
    for (mwSize i=0; i<num_vertices; i++){
        dsets.make_set(i);
    }

	// output array
    mxArray * label;
    mwSize label_num_dims=marker_num_dims;
    mwSize label_dims[label_num_dims];
    for (mwSize i=0; i<label_num_dims; i++){
        label_dims[i]=marker_dims[i];
    }
	plhs[0] = mxCreateNumericArray(label_num_dims,label_dims,mxUINT32_CLASS,mxREAL);
    label=plhs[0];
	uint32_t * label_data = (uint32_t *) mxGetData(label);
    mwSize label_num_elements=mxGetNumberOfElements(label);

	// initialize output array and find representatives of each class
	std::map<uint32_t,mwSize> components;
    for (mwSize i=0; i<label_num_elements; i++){
        label_data[i]=marker_data[i];
		if (label_data[i] > 0){
			components[label_data[i]] = i;
		}
    }

	// merge vertices labeled with the same marker
    for (mwSize i=0; i<label_num_elements; i++){
		if (label_data[i] > 0){
			dsets.union_set(components[label_data[i]],i);
		}
    }

	// sort the list of edges
    std::priority_queue <mwSize, vector<mwSize>, mycomp > pqueue (conn_data);
    for (mwSize iEdge=0; iEdge<conn_num_elements; iEdge++){
		if (conn_data[iEdge] > low_threshold){
			// check to see if either vertex attached to this edge is grow-able
			mwSize edge_array[conn_num_dims];
			ind2sub(iEdge,conn_num_dims,conn_dims,edge_array);
			mwSize v1, v2;
			mwSize v1_array[conn_num_dims-1], v2_array[conn_num_dims-1];
			for (mwSize i=0; i<conn_num_dims-1; i++){
				v1_array[i]=edge_array[i];
				v2_array[i]=edge_array[i];
			}
			for (mwSize i=0; i<nhood_dims[1]; i++){
				v2_array[i]+= (mwSize) nhood_data[nhood_dims[0]*i+edge_array[conn_num_dims-1]];
			}
            v1=sub2ind(v1_array, conn_num_dims-1, conn_dims);
            v2=sub2ind(v2_array, conn_num_dims-1, conn_dims);
			if (grow_mask_data[v1] || grow_mask_data[v2]){
		        pqueue.push(iEdge);
			}
		}
    }

    while (!pqueue.empty()){
        mwSize cur_edge=pqueue.top();
        pqueue.pop();
        mwSize edge_array[conn_num_dims];
        ind2sub(cur_edge,conn_num_dims,conn_dims,edge_array);
        mwSize v1, v2;
        mwSize v1_array[conn_num_dims-1], v2_array[conn_num_dims-1];
        for (mwSize i=0; i<conn_num_dims-1; i++){
            v1_array[i]=edge_array[i];
            v2_array[i]=edge_array[i];
        }
        for (mwSize i=0; i<nhood_dims[1]; i++){
            v2_array[i]+= (mwSize) nhood_data[nhood_dims[0]*i+edge_array[conn_num_dims-1]];
        }
        bool OOB=false;
        for (mwSize i=0; i<conn_num_dims-1; i++){
            if (v2_array[i]<0 || v2_array[i]>=conn_dims[i]){
                OOB=true;
            }
        }
        if (!OOB){
            v1=sub2ind(v1_array, conn_num_dims-1, conn_dims);
            v2=sub2ind(v2_array, conn_num_dims-1, conn_dims);
            mwSize set1=dsets.find_set(v1);
            mwSize set2=dsets.find_set(v2);
			uint32_t label_of_set1 = label_data[set1];
			uint32_t label_of_set2 = label_data[set2];
			if ((set1!=set2)
					&& (((label_of_set1 == 0) && grow_mask_data[set1])
						|| ((label_of_set2 == 0) && grow_mask_data[set2]))
					){
				dsets.link(set1, set2);
				// funkiness: either label_of_set1 is 0 or label_of_set2 is 0.
				// so the sum of the two values should the value of the non-zero
				// using this trick to find the new label
				label_data[dsets.find_set(set1)] = label_of_set1+label_of_set2;
            }
        }
    }

	// write out the final coloring
	for (mwSize i=0; i<label_num_elements; i++){
		label_data[i] = label_data[dsets.find_set(i)];
	}
}
