#ifndef OM_MST_H
#define OM_MST_H

#include "datalayer/omDataWrapper.h"
#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/fs/omIOnDiskFile.h"
#include "datalayer/fs/omFileQT.hpp"
#include "segment/io/omMSTtypes.h"

class OmSegmentation;

class OmMST {
private:
	static const double defaultThresholdSize_ = 0.9;

public:
	OmMST(OmSegmentation* segmentation);
	~OmMST(){}

	static double DefaultThresholdSize(){
		return defaultThresholdSize_;
	}

	void Read();
	void Flush();

	void Import(const std::vector<OmMSTImportEdge>& edges);

	bool isValid(){
		return numEdges_ > 0;
	}

	uint32_t NumEdges() const {
		return numEdges_;
	}

	double UserThreshold() const {
		return userThreshold_;
	}

	void SetUserThreshold(const double t) {
		userThreshold_ = t;
	}

	OmMSTEdge* Edges(){
		return edges_;
	}

private:
	OmSegmentation *const segmentation_;
	uint32_t numEdges_;
	double userThreshold_;

	boost::shared_ptr<OmIOnDiskFile<OmMSTEdge> > edgesPtr_;
	OmMSTEdge* edges_;

	void create();
	void convert();

	typedef OmFileReadQT<OmMSTEdge> reader_t;
	typedef OmFileWriteQT<OmMSTEdge> writer_t;

	std::string memMapPath();

	friend class SegmentTests1;

	friend class OmDataArchiveProject;
	friend QDataStream &operator<<(QDataStream& out, const OmSegmentation& seg);
};

#endif
