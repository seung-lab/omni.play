#include "segment/lowLevel/omSegmentGraphParallelComputing.h"
#include "segment/omSegment.h"

int OmSegmentGraphParallelComputing::encodeBatch(point batch_)
{
	return (batch_.x-1)*189 + (batch_.y-1)*9 + batch_.z - 1; // There are 9 possibilities for batch_.x (1-9), 21 for batch_.y (1-21) and 9 for
	// batch_.z (1-9). For every combination there is a unique number that corresponds to it.
}

int OmSegmentGraphParallelComputing::checkBatch(OmSegment *segment, int _step)
{
	point center_,batch_;

	center_.x = segment->GetData()->bounds.getCenter().x; // Extracting the center of the current segment
    center_.y = segment->GetData()->bounds.getCenter().y;
    center_.z = segment->GetData()->bounds.getCenter().z;

    batch_.x = center_.x/_step + (center_.x%_step)?1:0; // Determine which batch the current segment belongs to
    batch_.y = center_.y/_step + (center_.y%_step)?1:0;
    batch_.z = center_.z/_step + (center_.z%_step)?1:0;

	return encodeBatch(batch_);
}

void OmSegmentGraphParallelComputing::HandleInitialBatches(bool flag, boost::unordered_map <OmSegID,OmSegment*> *accessToSegments_, int batchID, OmSegmentGraph *graph_, OmMST *mst)
{
	const double stopThreshold = mst->UserThreshold();

	int i,j;
	OmMSTEdge *currEdge;
	for ( i=0; i<edgesInBatch[flag][batchID].size(); i++ )
	{
			currEdge = edgesInBatch[flag][batchID][i];

			OmSegment *segment1,*segment2;
			segment1 = (*accessToSegments_)[  (* currEdge ).node1ID  ];
			segment2 = (*accessToSegments_)[  (* currEdge ).node2ID  ];
			if ( checkBatch( segment1, step_ ) == checkBatch( segment2, step_ ) )
			{
				//Handle current edge

				if( 1 == (* currEdge ).userSplit ){
		            continue;
		        }

		        if( (* currEdge ).threshold >= stopThreshold ||
		            1 == (* currEdge ).userJoin )
		        { // join
		            if( 1 == (* currEdge ).wasJoined ){
		                continue;
		            }
		            if( (*graph_).joinInternal((* currEdge ).node2ID,
		                             (* currEdge ).node1ID,
		                             (* currEdge ).threshold, i) ){
		                (* currEdge ).wasJoined = 1;
		            } else {
		                (* currEdge ).userSplit = 1;
		            }
		        } else { // split
		            if( 0 == (* currEdge ).wasJoined ){
		                continue;
		            }
		            if( (*graph_).splitChildFromParentInternal((* currEdge ).node1ID)){
		                (* currEdge ).wasJoined = 0;
		            } else {
		                (* currEdge ).userJoin = 1;
		            }
		        }
			}
			else
			{
				edgesInBatch[flag^1][ checkBatch(segment1,(step_<<1) ) ].push_back(currEdge); // Add the current edge to the list of edges in that batch
			}
	}

	edgesInBatch[flag][batchID].clear();
}

void OmSegmentGraphParallelComputing::MergeBatches(OmSegmentGraph *graph_, boost::unordered_map <OmSegID,OmSegment*> *accessToSegments_, OmSegmentGraph::some_type* distribution_, OmMST *mst, OmSegmentGraph::AdjacencyMap* AdjacencyList)
{
	OmMSTEdge* edges = mst->Edges();
	int i;
	bool flag=0;
	point it,batchID;

	step_ = step;

	for ( i=0; i<mst->NumEdges(); i++ )
	{	
		OmSegment *segment1,*segment2;
		segment1 = (*accessToSegments_)[  edges[i].node1ID  ];

		edgesInBatch[0][ checkBatch( segment1, step_ ) ].push_back( &edges[i] );
	}

	while ( 1 )
	{
		for ( it.x = step_; it.x <= 900; it.x += step_ ) //----- Traversing the distribution array
            for ( it.y = step_; it.y <= 2100; it.y += step_ )
                for ( it.z = step_; it.z <= 900; it.z += step_ )
                {
                	batchID.x = it.x/step_;
                	batchID.y = it.y/step_;
                	batchID.z = it.z/step_;
                	HandleInitialBatches(flag, accessToSegments_, encodeBatch(batchID), graph_, mst); // Make in seperate threads

                }

        step_ = (step_<<1);
        flag ^= 1;
	}
}