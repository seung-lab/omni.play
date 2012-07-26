#include "segment/lowLevel/omSegmentGraphInitialLoad.hpp"
#include "segment/lowLevel/omSegmentsImplLowLevel.h"
#include "segment/lowLevel/omSegmentGraph.h"
#include "segment/lowLevel/omSegmentSelection.hpp"
#include "utility/omStringHelpers.h"
#include "volume/omSegmentation.h"
#include "segment/omSegmentSelector.h"

OmSegmentGraph::OmSegmentGraph()
    : segmentation_(NULL)
    , mCache(NULL)
    , segmentPages_(NULL)
    , segmentListsLL_(NULL)
{
    segsTempVec_.reserve(100);
}

OmSegmentGraph::~OmSegmentGraph()
{}

void OmSegmentGraph::RefreshGUIlists(){
    segmentListsLL_->RefreshGUIlists();
}

bool OmSegmentGraph::DoesGraphNeedToBeRefreshed( const uint32_t maxValue )
{
    return !forest_ || forest_->Size() != maxValue+1;
}

void OmSegmentGraph::Initialize(OmSegmentation* segmentation,
                                OmSegmentsImplLowLevel* cache)
{
    segmentation_ = segmentation;
    validGroupNum_ = segmentation->ValidGroupNum();
    mCache = cache;
    segmentPages_ = cache->SegmentStore();

    // maxValue is a valid segOmSegmentGraphInitialLoadment id, so array needs to be 1 bigger
    const uint32_t size = 1 + mCache->getMaxValue();

    forest_.reset(new OmDynamicForestCache(size));
    children_.reset(new OmSegmentChildren(size));

    validGroupNum_->Resize(size);

    segmentListsLL_ = segmentation->SegmentLists()->LowLevelList();
    segmentListsLL_->Init(cache, size);
}

void OmSegmentGraph::GrowGraphIfNeeded(OmSegment* seg)
{
    // maxValue is a valid segment id, so array needs to be 1 bigger
    const uint32_t size = 1 + mCache->getMaxValue();

    forest_->Resize(size);
    children_->Resize(size);
    segmentListsLL_->Resize(size);
    segmentListsLL_->AddSegment(seg);
    segmentListsLL_->ForceRefreshGUIlists();
}

void OmSegmentGraph::SetGlobalThreshold(OmMST* mst)
{
    OmSegmentGraphInitialLoad loader(forest_.get(),
                                     validGroupNum_,
                                     segmentListsLL_,
                                     segmentPages_,
                                     children_.get(), 
                                     &adjacencyList_,
                                     &orderOfAdding,
                                     &distribution_,
                                     &accessToSegments_);

    loader.SetGlobalThreshold(mst);
}

void OmSegmentGraph::ResetSizeThreshold(OmMST* mst)
{
    std::cout << "Got here\n";

    int direction = mst->UserSizeThresholdDirection();
    
    if ( !direction ) return;

    std::cout << "The direction is " << direction << std::endl;


    if ( direction == 1 ) ResetSizeThresholdUp(mst);
    else { std::cout<<"-----------\n"; ResetSizeThresholdDown(mst); std::cout<<"T-------------\n";} 

    std::cout << "Got here\n";
}

void OmSegmentGraph::ResetGlobalThreshold(OmMST* mst)
{

    std::cout << "\t" << om::string::humanizeNum(mst->NumEdges())
              << " edges..." << std::flush;

    OmTimer timer;

    forest_->SetBatch(true);
    forest_->ClearCache();

    const double stopThreshold = mst->UserThreshold();
    OmMSTEdge* edges = mst->Edges();

    for(uint32_t i = 0; i < mst->NumEdges(); ++i) {

        if( 1 == edges[i].userSplit ){
            continue;
        }

        if(edges[i].threshold >= stopThreshold ||
            1 == edges[i].userJoin )
        { // join
            if( 1 == edges[i].wasJoined ){
                continue;
            }
            if( joinInternal(edges[i].node2ID,
                             edges[i].node1ID,
                             edges[i].threshold, i) ){
                edges[i].wasJoined = 1;
            } else {
                edges[i].userSplit = 1;
            }
        } else { // split
            if( 0 == edges[i].wasJoined ){
                continue;
            }
            if( splitChildFromParentInternal(edges[i].node1ID)){
                edges[i].wasJoined = 0;
            } else {
                edges[i].userJoin = 1;
            }
        }
    }

    forest_->SetBatch(false);

    timer.PrintDone();
}

void OmSegmentGraph::ResetSizeThresholdUp(OmMST* mst) // Joining the edges that can be joined
{

    OmTimer timer;

    forest_->SetBatch(true);
    forest_->ClearCache();

    const double sizeThreshold = mst->UserSizeThreshold();
    const double stopThreshold = mst->UserThreshold();
    OmMSTEdge* edges = mst->Edges();
    
    for(uint32_t i = 0; i < mst->NumEdges(); i++)
    {
        if ( edges[i].threshold < stopThreshold ) break;

        if( 1 == edges[i].userSplit )
        {
            continue;
        }

        if ( sizeCheck(edges[i].node1ID,edges[i].node2ID,sizeThreshold) )
        { // join
            if( 1 == edges[i].wasJoined )
            {
                continue;
            }
            if( joinInternal(edges[i].node2ID,
                             edges[i].node1ID,
                             edges[i].threshold, i) )
            {
                edges[i].wasJoined = 1;
            }
            else
            {
                edges[i].userSplit = 1;
            }
        }
    }

    forest_->SetBatch(false);

    timer.PrintDone();
}

void OmSegmentGraph::ResetSizeThresholdDown(OmMST* mst) // Splitting the edges that should be split
{

    std:: cout << "Here we die\n";
    OmTimer timer;

    forest_->SetBatch(true);
    forest_->ClearCache();

    const double sizeThreshold = mst->UserSizeThreshold();
    OmMSTEdge* edges = mst->Edges();

    std::cout << "Not crashed yet\n";
    
    for(int32_t i = static_cast<int32_t>(mst->NumEdges())-1; i >= 0; --i)
    {
        //std::cout << i << '\n' << std::flush;

        if( 1 == edges[i].userSplit )
        {
            continue;
        }

        //std:: cout << i << " Not crashed yet 1\n";
        if ( !sizeCheck(edges[i].node1ID,edges[i].node2ID,sizeThreshold) )
        { 
            //std:: cout << i << " Not crashed yet 2\n";

            // split
            if( 0 == edges[i].wasJoined )
            {
                continue;
            }
            if( splitChildFromParentInternal(edges[i].node1ID))
            {
                edges[i].wasJoined = 0;
            }
            else
            {
                edges[i].userJoin = 1;
            }
        }
       //std:: cout << i << " Not crashed yet 3\n";
    }

    //std:: cout << "Not crashed yet !!!\n";

    forest_->SetBatch(false);

    timer.PrintDone();
}

bool OmSegmentGraph::sizeCheck(const OmSegID a, const OmSegID b, const double threshold)
{
    return threshold == 0 ||
           ((segmentListsLL_->GetSizeWithChildren(Root(a)) +
             segmentListsLL_->GetSizeWithChildren(Root(b))) < threshold);
}

bool OmSegmentGraph::joinInternal(const OmSegID parentID,
                                  const OmSegID childUnknownDepthID,
                                  const double threshold,
                                  const int edgeNumber)
{
    const OmSegID childRootID = Root(childUnknownDepthID);
    const OmSegID parentRootID = Root(parentID);

    if(childRootID == parentRootID){
        return false;
    }

    if(!validGroupNum_->InSameValidGroup(childRootID, parentID)){
        return false;
    }

    Join(childRootID, parentID);

    OmSegment* childRoot = segmentPages_->GetSegmentUnsafe(childRootID);
    OmSegment* parent = segmentPages_->GetSegmentUnsafe(parentID);

    children_->AddChild(parent, childRoot);
    childRoot->setParent(parent, threshold);
    childRoot->setEdgeNumber(edgeNumber);

    OmSegment* parentRoot = mCache->FindRoot(parent);

    parentRoot->touchFreshnessForMeshes();

    //UpdateSizeListsFromJoin(parentRoot, childRoot );
    segmentListsLL_->UpdateSizeListsFromJoin(parentRoot, childRoot);

    return true;
}


bool OmSegmentGraph::splitChildFromParentInternal( const OmSegID childID )
{
    OmSegment* child = mCache->SegmentStore()->GetSegment( childID );

    if( child->getThreshold() > 1 ){
        return false;
    }

    OmSegment* parent = child->getParent();
    if(!parent){ // user manually split?
        return false;
    }

    if( child->IsValidListType() || parent->IsValidListType()){
        return false;
    }

    children_->RemoveChild(parent, child);
    Cut(childID);
    child->setParent(NULL); // TODO: also set threshold??
    child->setEdgeNumber(-1);

    OmSegment* parentRoot = mCache->FindRoot(parent);

    parentRoot->touchFreshnessForMeshes();
    child->touchFreshnessForMeshes();

    UpdateSizeListsFromSplit(parentRoot, child);

    return true;
}

void OmSegmentGraph::UpdateSizeListsFromJoin(OmSegment* parent, OmSegment* child)
{
    OmSegment* root = mCache->FindRoot(parent);
    segmentListsLL_->UpdateSizeListsFromJoin(root, child);
}

void OmSegmentGraph::UpdateSizeListsFromSplit(OmSegment* parent, OmSegment* child)
{
    OmSegment* root = mCache->FindRoot(parent);

    const SizeAndNumPieces childInfo =
        computeSegmentSizeWithChildren(child);

    segmentListsLL_->UpdateSizeListsFromSplit(root, child, childInfo);
}

 double OmSegmentGraph::SizeOfBFSGrowth(OmMST* mst, OmSegID SegmentID, double threshold)
 {
     double totalSize = 0;

     std::queue <OmSegID> q;
     OmMSTEdge *currEdge;
     OmSegID currSegment, nextSegment;
    
     q.push( SegmentID );
     totalSize += segmentListsLL_->GetSizeWithChildren(Root( SegmentID )); // PROBABLY SHOULD BE JUST THE CURRENT SEGMENT WITHOUT THE CHILDREN

     boost::unordered_map <OmSegID,bool> used;
     used[ SegmentID ] = 1; //setToAdd.insert ( SegmentID );

     while (!q.empty())
     {

         currSegment = q.front();

         q.pop();

         for ( int i = 0; i < adjacencyList_[currSegment].size(); i++ )
         {
             currEdge = adjacencyList_[currSegment][i];

             if ( (*currEdge).threshold < threshold ) continue;

             if ( currSegment == (*currEdge).node2ID ) nextSegment = (*currEdge).node1ID;
             else nextSegment = (*currEdge).node2ID;

             if ( used[ nextSegment ] ) continue; //if ( setToAdd.find( nextSegment ) != setToAdd.end() ) continue;

             q.push( nextSegment );
             used[ nextSegment ] = 1; //setToAdd.insert ( nextSegment );
             totalSize += segmentListsLL_->GetSizeWithChildren(Root( nextSegment)); // PROBABLY SHOULD BE JUST THE CURRENT SEGMENT WITHOUT THE CHILDREN
         }
     }

     return totalSize;
 }

 void OmSegmentGraph::Grow_LocalSizeThreshold(OmMST* mst, OmSegmentSelector* sel, OmSegID SegmentID)
 {
     double totalSize,sizeThreshold = mst->UserSizeThreshold();

     //std::cout << "The size of the current segment is " << segmentListsLL_->GetSizeWithChildren(Root(SegmentID)) << std::endl;

     double l=0.6,r=1,mid;
     while ( (r-l) > 0.0001 )
     {
         mid = ( ( l + r )/2 );
         totalSize = SizeOfBFSGrowth (mst,SegmentID,mid);

         //std::cout.precision(4);
         //std::cout << std::fixed << l << ' ' << std::fixed << r << " The total size is " << totalSize << std::endl;
         
         if ( totalSize > sizeThreshold ) l = mid;
         else r = mid;
     }

     mst->SetUserASThreshold(r);

     AddSegments_BreadthFirstSearch(mst,sel,SegmentID);
 }

void OmSegmentGraph::AddNeighboursToSelection(OmMST* mst, OmSegmentSelector* sel, OmSegID SegmentID)
{
    OmSegID nextSegment;

    OmMSTEdge *currEdge;

    double threshold = mst->UserASThreshold();

    std::vector<OmSegID> vecToAdd;
    boost::unordered_set<OmSegID> setToAdd;
    setToAdd.insert ( SegmentID );
    vecToAdd.push_back (SegmentID);

    //std::cout << "Adding Neighbors to ____: " << SegmentID << " with " << adjacencyList_[SegmentID].size() << " neighbours" << endl;

    for ( int i = 0; i < adjacencyList_[SegmentID].size() ; i++)
    {
        currEdge = adjacencyList_[SegmentID][i];

        if ( (*currEdge).threshold < threshold ) continue;

        if ( SegmentID == (*currEdge).node2ID ) nextSegment = (*currEdge).node1ID;
        else nextSegment = (*currEdge).node2ID;
        
        if ( sel->IsSegmentSelected ( nextSegment ) ) continue;
        if ( setToAdd.find( nextSegment ) != setToAdd.end() ) continue;

        //std::cout << "Adding " << (*currEdge).node2ID << endl;

        setToAdd.insert ( nextSegment );
        vecToAdd.push_back ( nextSegment );
    }

    sel->InsertSegmentsOrdered (&vecToAdd);
    sel->sendEvent();
}


void OmSegmentGraph::AddSegments_BreadthFirstSearch(OmMST* mst, OmSegmentSelector* sel, OmSegID SegmentID)
{
    std::queue <OmSegID> q;
    OmMSTEdge *currEdge;
    OmSegID currSegment, nextSegment;
    double threshold = mst->UserASThreshold();
    
    q.push( SegmentID );

    std::vector<OmSegID> vecToAdd;
    boost::unordered_set<OmSegID> setToAdd;
    setToAdd.insert ( SegmentID );
    vecToAdd.push_back(SegmentID);

    int br=0;
    while (!q.empty())
    {
        br++;

        currSegment = q.front();

        if ( br == 1000 ) break;

        q.pop();

        for ( int i = 0; i < adjacencyList_[currSegment].size(); i++ )
        {
            currEdge = adjacencyList_[currSegment][i];

            if ( (*currEdge).threshold < threshold ) continue;

            if ( currSegment == (*currEdge).node2ID ) nextSegment = (*currEdge).node1ID;
            else nextSegment = (*currEdge).node2ID;

            if ( sel->IsSegmentSelected( nextSegment ) ) continue;
            if ( setToAdd.find( nextSegment ) != setToAdd.end() ) continue;

            q.push( nextSegment );
            setToAdd.insert ( nextSegment );
            vecToAdd.push_back ( nextSegment );
        }
    }
    sel->InsertSegmentsOrdered (&vecToAdd);
    sel->sendEvent();
}

void OmSegmentGraph::Trim(OmMST* mst, OmSegmentSelector* sel, OmSegID SegmentID)
{

    //std::cout << "Trimming from segment " << SegmentID << std::endl;

    OmMSTEdge *currEdge;
    OmSegID currSegment, nextSegment;

    uint32_t mini = -1;

    if ( !orderOfAdding[SegmentID] )
    {
        for ( int i = 0; i < adjacencyList_[SegmentID].size(); i++ )
        {
            currEdge = adjacencyList_[SegmentID][i];

           if ( SegmentID == (*currEdge).node2ID ) nextSegment = (*currEdge).node1ID;
                else nextSegment = (*currEdge).node2ID;

            if ( orderOfAdding[nextSegment] &&
                ( mini == -1 || orderOfAdding[nextSegment] < mini ) )
                mini = orderOfAdding[nextSegment];
        }
    }
    else mini = orderOfAdding[SegmentID];

 // std::cout << "---- " << mini << std::endl;

    std::queue <OmSegID> q;
    
    q.push( SegmentID );

    boost::unordered_set<OmSegID> setToRemove;

    while (!q.empty())
    {
        currSegment = q.front();

        q.pop();

        for ( int i = 0; i < adjacencyList_[currSegment].size(); i++ )
        {
            currEdge = adjacencyList_[currSegment][i];

            if ( currSegment == (*currEdge).node2ID ) nextSegment = (*currEdge).node1ID;
            else nextSegment = (*currEdge).node2ID;

          //  std::cout << i << " Segment " << nextSegment << ' ' << orderOfAdding[nextSegment] << std::endl;
            if ( orderOfAdding[nextSegment] <= mini ) continue;
          //  std::cout << i << std::endl;

            if ( ! sel->IsSegmentSelected( nextSegment ) ) continue;
            if ( setToRemove.find( nextSegment ) != setToRemove.end() ) continue;

            q.push( nextSegment );

           // std::cout << "Adding to the set\n";
            setToRemove.insert ( nextSegment );
        }
    }

//    std::cout << "The set has " << setToRemove.size() << " elements\n";
    sel->RemoveTheseSegments (&setToRemove);
    sel->sendEvent();
}

void OmSegmentGraph::AddSegments_BFS_DynamicThreshold(OmMST* mst, OmSegmentSelector* sel, OmSegID SegmentID)
{
    std::queue <OmSegID> q;
    OmMSTEdge *currEdge;
    OmSegID currSegment, nextSegment;
    double threshold = mst->UserASThreshold(),difference,top;
    
    q.push( SegmentID );

    std::vector<OmSegID> vecToAdd;
    boost::unordered_set<OmSegID> setToAdd;
    setToAdd.insert ( SegmentID );
    vecToAdd.push_back (SegmentID);

    int br=0,sizeList;
    while (!q.empty())
    {
        br++;

        currSegment = q.front();

        if ( br == 1000 ) break;

        q.pop();

        sizeList = adjacencyList_[currSegment].size();
        difference = (*adjacencyList_[currSegment][0]).threshold - (*adjacencyList_[currSegment][ sizeList - 1 ]).threshold;
        if ( sizeList == 1 ) top = 0;
        //top = difference / double(sizeList-1);

        for ( int i = 0; i < sizeList; i++)
        {

            if ( (*adjacencyList_[currSegment][0]).threshold - (*adjacencyList_[currSegment][i]).threshold > 0.05 ) break;

            currEdge = adjacencyList_[currSegment][i];

            //if ( (*currEdge).threshold < threshold ) continue;
            
            if ( currSegment == (*currEdge).node2ID ) nextSegment = (*currEdge).node1ID;
            else nextSegment = (*currEdge).node2ID;

            if ( sel->IsSegmentSelected( nextSegment ) ) continue;

            if ( setToAdd.find( nextSegment ) != setToAdd.end() ) continue;

            q.push( nextSegment );
            setToAdd.insert ( nextSegment );
            vecToAdd.push_back (nextSegment);
        }
    }
    sel->InsertSegmentsOrdered (&vecToAdd);
    sel->sendEvent();
}

void OmSegmentGraph::AddSegments_DepthFirstSearch(OmMST* mst, OmSegmentSelector* sel, OmSegID SegmentID)
{
    std::vector<OmSegID> stackDFS(1000); // = new OmSegID[1000];
    stackDFS[0] = 1;

    std::vector <OmSegID> vecToAdd;
    boost::unordered_set<OmSegID> setToAdd;
    setToAdd.insert ( SegmentID );
    vecToAdd.push_back ( SegmentID );

    OmMSTEdge *currEdge;
    OmSegID currSegment, nextSegment;
    double threshold = mst->UserASThreshold();

    stackDFS[1] = SegmentID;

    while (stackDFS[0])
    {
        currSegment = stackDFS[ stackDFS[0] ];
        stackDFS[0]--;

        for ( int i = 0; i < adjacencyList_[currSegment].size(); i++ )
        {
            currEdge = adjacencyList_[currSegment][i];

            if ( (*currEdge).threshold < threshold ) continue;

            if ( currSegment == (*currEdge).node2ID ) nextSegment = (*currEdge).node1ID;
            else nextSegment = (*currEdge).node2ID;

            if ( sel->IsSegmentSelected( nextSegment ) ) continue;
            if ( setToAdd.find( nextSegment ) != setToAdd.end() ) continue;

            stackDFS[0]++;
            stackDFS[ stackDFS[0] ] = nextSegment;

            setToAdd.insert ( nextSegment );
            vecToAdd.push_back ( nextSegment );
        }
    }

    sel->InsertSegmentsOrdered (&vecToAdd);
    sel->sendEvent();
}

SizeAndNumPieces
OmSegmentGraph::computeSegmentSizeWithChildren(OmSegment* inSeg)
{
    int64_t numVoxels = 0;
    int32_t numPieces = 0;

    segsTempVec_.push_back(inSeg);

    while(!segsTempVec_.empty())
    {
        OmSegment* segRet = segsTempVec_.back();
        segsTempVec_.pop_back();

        FOR_EACH(iter, children_->GetChildren(segRet)){
            segsTempVec_.push_back(*iter);
        }

        const int64_t segSize = segRet->size();

        assert(segSize >= 0);

        numVoxels += segSize;

        assert(numVoxels >= 0);

        ++numPieces;
    }

    SizeAndNumPieces ret = { numVoxels, numPieces };
    return ret;
}