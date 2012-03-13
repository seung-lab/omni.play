#pragma once

#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataWrapper.h"
#include "segment/io/omMST.h"
#include "segment/omSegments.h"
#include "volume/omSegmentation.h"

class OmMSTImportHdf5 {
private:
    OmSegmentation *const vol_;
    std::vector<OmMSTImportEdge> edges_;

public:
    OmMSTImportHdf5(OmSegmentation* vol)
        : vol_(vol)
    {}

    bool Import(const std::string& fname)
    {
        if(!readFromHDF5(fname)){
            return false;
        }

        checkSizes();

        process();

        checkSegmentIDs();

        vol_->MST()->Import(edges_);

        return true;
    }

private:
    OmDataWrapperPtr dend_;
    Vector3i dendSizes_; // example:  2 x 13,672

    OmDataWrapperPtr dendValues_;
    Vector3i dendValuesSizes_; // example:  13,672 x 0

    bool readFromHDF5(const std::string& fname)
    {
        OmHdf5* hdf5 = OmHdf5Manager::Get(fname, true);
        hdf5->open();

        if(!importDend(hdf5)){
            return false;
        }

        if(!importDendValues(hdf5)){
            return false;
        }

        hdf5->close();

        return true;
    }

    void checkSizes()
    {
        if(dendSizes_.y != dendValuesSizes_.x){
            throw OmIoException("MST size mismatch");
        }
    }

    void process()
    {
        const uint32_t numEdges = edges_.size();

        uint32_t const*const nodes = dend_->getPtr<uint32_t>();
        float const*const thresholds = dendValues_->getPtr<float>();

        for(uint32_t i = 0; i < numEdges; ++i)
        {
            edges_[i].node1ID = nodes[i];
            edges_[i].node2ID = nodes[i + numEdges];
            edges_[i].threshold = thresholds[i];
        }
    }

    bool importDend(OmHdf5* hdf5)
    {
        OmDataPath datasetName("dend");

        if(!hdf5->dataset_exists(datasetName)){
            printf("no dendrogram dataset found\n");
            return false;
        }

        dendSizes_ = hdf5->getDatasetDims(datasetName);

        const int numEdges = dendSizes_.y;
        edges_.resize(numEdges);

        int numBytes;
        dend_ = hdf5->readDataset(datasetName, &numBytes);

        printf("\tdendrogram is %s x %s (%s bytes)\n",
               om::string::humanizeNum(dendSizes_.x).c_str(),
               om::string::humanizeNum(dendSizes_.y).c_str(),
               om::string::humanizeNum(numBytes).c_str());

        return true;
    }

    bool importDendValues(OmHdf5* hdf5)
    {
        OmDataPath datasetName("dendValues");

        if(!hdf5->dataset_exists(datasetName)){
            printf("no dendrogram values dataset found\n");
            return false;
        }

        dendValuesSizes_ = hdf5->getDatasetDims(datasetName);

        int numBytes;
        dendValues_ = hdf5->readDataset(datasetName, &numBytes);

        printf("\tdendrogram values is %s x %s (%s bytes)\n",
               om::string::humanizeNum(dendValuesSizes_.x).c_str(),
               om::string::humanizeNum(dendValuesSizes_.y).c_str(),
               om::string::humanizeNum(numBytes).c_str());

        return true;
    }

    void checkSegmentIDs()
    {
        const uint32_t size = edges_.size();

        std::vector<OmMSTImportEdge> valid;
        valid.reserve(size);

        OmSegments* segments = vol_->Segments();

        for(uint32_t i = 0; i < size; ++i)
        {
            if(segments->IsSegmentValid(edges_[i].node1ID))
            {
                if(segments->IsSegmentValid(edges_[i].node2ID))
                {
                    valid.push_back(edges_[i]);
                    continue;
                }
            }

            std::cout << "MST edge import: skipping: " << edges_[i] << "\n";
        }

        const uint32_t oldSize = edges_.size();
        const uint32_t newSize = valid.size();
        const uint32_t diff = oldSize - newSize;

        edges_.swap(valid);

        if(!diff){
            std::cout << "checked MST: all edges good!\n";

        } else {
            std::cout << "checked MST: "
                      << "removed " << diff  << " invalid edges; "
                      << newSize << " edges left\n";
        }
    }
};

