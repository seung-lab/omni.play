#ifndef OM_MST_IMPORT_HDF5_HPP
#define OM_MST_IMPORT_HDF5_HPP

#include "segment/io/omMST.h"
#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataWrapper.h"
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
		OmHdf5* hdf5 = OmHdf5Manager::Get(fname, true);
		hdf5->open();

		if(!importDend(hdf5)){
			return false;
		}

		if(!importDendValues(hdf5)){
			return false;
		}

		hdf5->close();

		checkSizes();
		process();

		vol_->MST()->Import(edges_);

		return true;
	}

private:
	OmDataWrapperPtr dend_;
	Vector3i dendSizes_; // example:  2 x 13,672

	OmDataWrapperPtr dendValues_;
	Vector3i dendValuesSizes_; // example:  13,672 x 0

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

		for(uint32_t i = 0; i < numEdges; ++i){
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
			   OmStringHelpers::CommaDeliminateNum(dendSizes_.x).c_str(),
			   OmStringHelpers::CommaDeliminateNum(dendSizes_.y).c_str(),
			   OmStringHelpers::CommaDeliminateNum(numBytes).c_str());

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
			   OmStringHelpers::CommaDeliminateNum(dendValuesSizes_.x).c_str(),
			   OmStringHelpers::CommaDeliminateNum(dendValuesSizes_.y).c_str(),
			   OmStringHelpers::CommaDeliminateNum(numBytes).c_str());

		return true;
	}
};

#endif
